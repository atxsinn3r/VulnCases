#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class Object {
public:
  void* buf = NULL;

  Object() {
    buf = malloc(1024);
  }

  ~Object() {
    printf("Destructor is triggered\n");
    free(buf);
    buf = NULL;
  }
};

void Test(Object obj) {
  printf("In Function Test()\n");
}

int main(int args, char** argv) {
  Object obj;
  Test(obj);
  return 0;
}
