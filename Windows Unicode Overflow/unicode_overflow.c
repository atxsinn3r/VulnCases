#include <Windows.h>
#include <stdio.h>

#define ORIGINAL_STRING_SIZE 1024


int main(int args, char** argv) {
  char originalStr[ORIGINAL_STRING_SIZE];
  memset(originalStr, 'A', ORIGINAL_STRING_SIZE);
  memcpy(originalStr+ORIGINAL_STRING_SIZE-1, "\0", 1);

  WCHAR newBuffer[32];
  memset(newBuffer, '\0', sizeof(newBuffer));
  MultiByteToWideChar(CP_ACP, 0, originalStr, -1, newBuffer, sizeof(newBuffer));
  wprintf(L"%ls\n", newBuffer);
  return 0;
} 