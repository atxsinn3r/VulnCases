#include <stdio.h>
#include <vector>
#include <Windows.h>
using namespace std;

/*
This demonstrates how a leaked pointer can calculate the base address of a process.
It mimics a use-after-free scenario where we have controlled of a freed chunk,
and gets converted to a type-confusion bug. This is achieved by allocating multiple
0x10-byte arrays on a heap, one of them is freed, and then we create another array
(which represents a fake object in terms of exploitation), which contains an object
instead of a string. When the object is being read, it is treated as a string, which
prints the pointer to the object. One the address of the object is known, we can
subtract a value that's the offset to the image base, and then we get the image
base address.

Tested on Windows 10 as of May 24th 2019
*/

#define ALLOC_COUNT 0x100

void TestArray();
void CorruptChunk(char*);
void HexDump (char*, void*, int);
void PrintData(LPVOID*);
void PrintBaseAddress(char*);

class SomeObject {
public:
  int x = 0;
  virtual void Function1() {
    printf("Function 1\n");
  }

  virtual void Function2() {
    printf("Function 2\n");
  }

  virtual void Function3() {
    printf("Function 3\n");
  }

  virtual void Function4() {
    printf("Function 4\n");
  }
};

void HexDump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

void TestArray() {
  LPVOID* chunks = new LPVOID[ALLOC_COUNT];
  int chunkToFree = ALLOC_COUNT-2;
  int i = 0;

  // Lay out the chunks and free one of them (in the middle)
  for (i; i < ALLOC_COUNT; i++) {
    // WinDBG tells us the size of the items object is 0x10
    char** items = new char*[4];
    items[0] = "aaaaaa";
    items[1] = "bbbbbb";
    items[2] = "cccccc";
    items[3] = "dddddd";

    chunks[i] = items;
    //printf("Chunk %04d | 0x%08x  0x%08x | 0: %s, 1: %s, 2: %s, 3: %s\n", i, &chunks[i], chunks[i], items[0], items[1], items[2], items[3]);
  }

  printf("Freeing chunk: 0x%08x\n", chunks[chunkToFree]);
  delete[] (char*) chunks[chunkToFree];

  i = 0;
  for (i; i < 16; i++) {
    char** fakeItems = new char*[4];
    char* obj = (char*) new SomeObject();
    fakeItems[0] = "AAAAAA";
    fakeItems[1] = "BBBBBB";
    fakeItems[2] = "CCCCCC";
    fakeItems[3] = obj;
    //printf("Fake chunk: 0x%08x\n", fakeItems);
  }

  PrintData(chunks);
  system("PAUSE");
}

inline void PrintBaseAddress(char* ref) {
  char buf[5];
  memset(buf, 0x00, 5);
  memcpy(buf, ref, 4);
  int num = int((unsigned char)(buf[3]) << 24 | (unsigned char)(buf[2]) << 16 | (unsigned char)(buf[1]) << 8 | (unsigned char)(buf[0]));
  // The offset is found by:
  // Finding the addreses of SomeObject
  // The address of the image base address
  int baseAddr = num - 0x0001028c;
  printf("\nImage base address is: 0x%08x\n", baseAddr);
}

void PrintData(LPVOID* chunks) {
  for (int i=0; i < ALLOC_COUNT; i++) {
    if (i == 254) {
      char** items = (char**) chunks[i];
      printf("Overwritten Chunk %04d | 0x%08x  0x%08x\n", i, &chunks[i], chunks[i], items[0]);
      HexDump("Item 0 in chunk", items[0], 32);
      HexDump("Item 1 in chunk", items[1], 32);
      HexDump("Item 2 in chunk", items[2], 32);
      // The address of SomeObject is found as the 4th item for the array.
      HexDump("Item 3 in chunk", items[3], 32);
      PrintBaseAddress(items[3]);
    }
  }
}

int main(void) {
  printf("Testing array\n");
  TestArray();
  return 0;
}
