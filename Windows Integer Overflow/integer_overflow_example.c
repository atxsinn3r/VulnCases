#include <stdio.h>

/*
Example of an integer overflow.

To compile (with Visual Studio):
cl /MT /GS- integer_overflow_example.c
*/

#define MAX_DATA_SIZE 255
#define DATA_SIZE 1000

// This struct represents a file or a network packet
struct MockedFile {
  char size[MAX_DATA_SIZE];
  char* data;
};

// Prepares the fake file
void MockFileFormat(struct MockedFile* mock) {
  char size[1];
  size[0] = DATA_SIZE;

  char* data = (char*) malloc(DATA_SIZE);
  memset(data, 'A', DATA_SIZE);

  memcpy(mock->size, size, 1);
  mock->data = data;
}

int main(int args, char** argv) {
  // Pretend this gives us the fake file (or a network packet)
  struct MockedFile mock;
  MockFileFormat(&mock);

  // The size field is copied to a one-byte buffer
  char sizeBuf[1];
  memcpy(sizeBuf, mock.size, 1);

  // char by default is signed, which means this check can only
  // handle range between -128 to 127, and may result an integer
  // overflow
  char size = sizeBuf[0];
  printf("Data size is %d\n", size);
  if (size < MAX_DATA_SIZE) {
    char buffer[MAX_DATA_SIZE];
    memset(buffer, '\0', MAX_DATA_SIZE);
    memcpy(buffer, mock.data, size);
    printf("%s\n", buffer);
  }

  return 0;
}