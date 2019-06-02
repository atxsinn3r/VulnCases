# Windows Heap Overflow Arbitrary Code Execution

This example demonstrates how to turn a heap overflow on Windows 10 into an exploitable crash.
The technique is similar to the leak example I used in a different folder. If you are compiling
the source code yourself, the position of the vftable can shift (in .rdata), so make sure that
offset to the image address is still correct.

Tested on Windows 10 as of June 2nd, 2019.