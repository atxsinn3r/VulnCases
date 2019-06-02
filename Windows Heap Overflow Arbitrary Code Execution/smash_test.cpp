#include <Windows.h>
#include <stdio.h>
#include <vector>
using namespace std;

#define CHUNK_SIZE 0x190
#define ALLOC_COUNT 10

class SomeObject {
public:
  void function1() {
  };
  virtual void virtualFunction() {
    printf("test\n");
  };
};

int main(int args, char** argv) {
  int i;
  HANDLE hChunk;
  void* allocations[ALLOC_COUNT];
  SomeObject* objects[5];
  SomeObject* obj = new SomeObject();
  printf("SomeObject address : 0x%08x\n", obj);
  int vectorSize = 40;

  HANDLE defaultHeap = GetProcessHeap();

  for (i = 0; i < ALLOC_COUNT; i++) {
    hChunk = HeapAlloc(defaultHeap, 0, CHUNK_SIZE);
    memset(hChunk, 'A', CHUNK_SIZE);
    allocations[i] = hChunk;
    printf("[%d] Heap chunk in backend : 0x%08x\n", i, hChunk);
  }

  HeapFree(defaultHeap, HEAP_NO_SERIALIZE, allocations[3]);
  vector<SomeObject*> v1(vectorSize, obj);
  vector<SomeObject*> v2(vectorSize, obj);
  vector<SomeObject*> v3(vectorSize, obj);
  vector<SomeObject*> v4(vectorSize, obj);
  vector<SomeObject*> v5(vectorSize, obj);
  vector<SomeObject*> v6(vectorSize, obj);
  vector<SomeObject*> v7(vectorSize, obj);
  vector<SomeObject*> v8(vectorSize, obj);
  vector<SomeObject*> v9(vectorSize, obj);
  vector<SomeObject*> v10(vectorSize, obj);

  printf("vector : 0x%08x\n", v1);
  printf("vector : 0x%08x\n", v2);
  printf("vector : 0x%08x\n", v3);
  printf("vector : 0x%08x\n", v4);
  printf("vector : 0x%08x\n", v5);
  printf("vector : 0x%08x\n", v6);
  printf("vector : 0x%08x\n", v7);
  printf("vector : 0x%08x\n", v8);
  printf("vector : 0x%08x\n", v9);
  printf("vector : 0x%08x\n", v10);

  memset(allocations[2], 'B', CHUNK_SIZE + 8 + 32);

  v1.at(0)->virtualFunction();

  system("PAUSE");
  return 0;
}
