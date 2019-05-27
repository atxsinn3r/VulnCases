# Windows Use After Free Type Confusion Info Leak

![Screenshot](https://github.com/wchen-r7/VulnCases/blob/master/Windows%20Use%20After%20Free%20to%20Type%20Confusion%20Leak/Screenshot.png?raw=true)

The title is quite a mouthful but this is what the bug is all about. The case study is intended
to demonstrate how a use after free condition could allow an application to confuse an object
as a string, as a result leaking the address when there is a read primitive. The leaked address
can also be used to calculate the image base address, which defeats the purpose of ASLR, and
collect ROP gadgets (which defeats DEP).
