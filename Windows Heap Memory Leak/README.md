# Windows Heap Memory Leak

This is an example of a heap memory leak for Windows, as a server application. Due to the fact
the malloc is never properly freed, typically it can be leveraged to exploit memory corruption
bugs in the real world.