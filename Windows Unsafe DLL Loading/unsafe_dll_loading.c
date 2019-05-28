#include <Windows.h>
#include <stdio.h>

#define MODULE_NAME "HelloWorld.dll"

int main(int args, char** argv) {
  // Read about search order:
  // https://docs.microsoft.com/en-us/windows/desktop/Dlls/dynamic-link-library-search-order
  // That's how they get you.
  HMODULE module = LoadLibrary(MODULE_NAME);
  if (module == NULL) {
    printf("Unable to load %s\n", MODULE_NAME);
    return -1;
  }

  printf("Press any key to unload %s\n", MODULE_NAME);
  system("PAUSE");
  if (!FreeLibrary(module)) {
    printf("Failed to unload\n");
  }

  return 0;
}