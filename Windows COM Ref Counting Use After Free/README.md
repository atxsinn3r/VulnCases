# Windows COM Reference Counting Use After Free

![Screenshot](https://raw.githubusercontent.com/wchen-r7/VulnCases/master/Windows%20COM%20Ref%20Counting%20Use%20After%20Free/Screenshot.png)

This is an example of a use after free vulnerability due to incorrect reference counting. This type
of use after free is more common among Windows binaries, such as Internet Explorer, ActiveX, etc.
The idea of reference counting is simple but kind of easy to make mistakes. Basically, every new
copy of a pointer to an interface on an object, you're supposed to AddRef:

```cpp
someObject->AddRef()
```

What that does is literally adding 1 to a counter.

When the pointer is no longer needed, then you do a Release():

```cpp
someObject->Release()
```

When the counter becomes 0, then the object will actually be freed.

If the counting is wrong, you may end up freeing the object unexpectedly.

In this folder, the ComClientExample.exe should demonstrate that problem, also EIP should be 0x41414141.
Remember to register ComServerExample.dll as admin:

```
regsvr32 ComServerExample.dll
```

Tested on Windows 10 as of May 25th 2019.
