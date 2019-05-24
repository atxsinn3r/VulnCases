#include <stdio.h>
#include <Windows.h>

/*
To compile with Visual Studio:
cl.exe /MT /EHa /GS- format_string_arbitrary_write.cpp

To read a pointer, your input can be:
%x

To modify the flag in SomeObject, you can do the following.
%.1s means to print one byte, and %n is to count, and save that result.
It is important you disable GS in order to do this.
%.1s%n
*/

class SomeObject {
public:
  // Let's change this flag to true with a format string attack.
  // When this is an object, the address for flag is the first DWORD of the allocation.
  BOOL flag;
  SomeObject() {
    flag = FALSE;
  }
};

int main(int args, char** argv) {
  if (args <= 1) {
    printf("Enter something to mess with the flag in the object\n");
    return 0;
  }

  // The %n format is disabled by default for Windows binaries, so we need to enable it. 
  _set_printf_count_output(1);

  // Watch where the address is for SomeObject on the stack.
  // It could be somewhere the printf function can reach, and end up abusing.
  SomeObject* obj = new SomeObject();
  printf(argv[1]);

  if (obj->flag) {
    printf("\nFlag is true: You have successfully modified the flag of the object\n");
  } else {
    printf("\nFlag is false: Try again\n");
  }
  delete(obj);
  obj = NULL;
  return 0;
}