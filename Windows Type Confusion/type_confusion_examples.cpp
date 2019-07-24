#include <stdio.h>
#include <Windows.h>

/*
Compiled using Visual Studio 2019:
cl.exe /Z7 /MT /EHa type_confusion_examples.cpp
*/

class Base {};

class Child : public Base {
public:
  virtual void test1();
};

class Baddie : public Base {
public:
  virtual void UnexpectedFunction() {
    printf("If you see this function being executed, you are confused!\n");
  }
};

void* CopyString(char* s) {
  void* buf = malloc(strlen(s));
  strcpy((char*) buf, s);
  return buf;
}

int main(int args, char** argv) {
  // Example 1 of type confusion
  Base* baddie = new Baddie();
  Child* child1 = static_cast<Child*>(baddie);
  if (child1)
    child1->test1();

  // Example 2 of type confusion
  char* s = "AAAAAAAAAAAAAAAA";
  void* buf = CopyString(s);
  Child* child2 = static_cast<Child*>(buf);
  child2->test1();
  free(buf);

  system("PAUSE");
  return 0;
}
