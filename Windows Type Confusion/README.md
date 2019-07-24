# Windows Type Confusion Example

Type confusion is when code doesn't verify the type of the object that is passed to it, and use it blindly without any type checking. In some cases, you find them as vulnerabilities. In some exploitation cases, type confusion is something you could create in order to gain code execution.

In file type_confusion_examples.cpp, there are two examples.

The first one demonstrates that the program intends to call function A, but when you feed it the wrong object, it ends up calling a different function because the vtable offset is the same. This would be more obvious if you look at the code in IDA. If you see the following message, that is it:

```
If you see this function being executed, you are confused!
```

The second example reminds us the fact that when an object is created, the `new` operator is used, and what that returns is really just pointer to a heap allocation (typically with HeapAlloc or malloc). If that is the type of scenario you can create (including content you control), you can easily cause an exploitable looking crash:

```
0:000> g
(24f4.1064): Access violation - code c0000005 (first chance)
First chance exceptions are reported before any exception handling.
This exception may be expected and handled.
*** WARNING: Unable to verify checksum for type_confusion_examples.exe
eax=000e0828 ebx=00a9f000 ecx=000e0828 edx=41414141 esi=000dd4f0 edi=000dd548
eip=00e771f6 esp=00cffb90 ebp=00cffbb4 iopl=0         nv up ei pl nz ac pe nc
cs=0023  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00010216
type_confusion_examples!main+0xa6:
00e771f6 8b02            mov     eax,dword ptr [edx]  ds:002b:41414141=????????
0:000> u
type_confusion_examples!main+0xa6 [C:\Users\sinn3r\Desktop\type_confusion_examples.cpp @ 40]:
00e771f6 8b02            mov     eax,dword ptr [edx]
00e771f8 ffd0            call    eax
00e771fa 8b4de0          mov     ecx,dword ptr [ebp-20h]
00e771fd 51              push    ecx
00e771fe e85bb8ffff      call    type_confusion_examples!ILT+6745(_free) (00e72a5e)
00e77203 83c404          add     esp,4
00e77206 6864deec00      push    offset type_confusion_examples!__xt_z+0x11c (00ecde64)
00e7720b e803aeffff      call    type_confusion_examples!ILT+4110(_system) (00e72013)
```







