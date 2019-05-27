# Windows Unicode Overflow

![Screenshot](https://raw.githubusercontent.com/wchen-r7/VulnCases/master/Windows%20Unicode%20Buffer%20Overflow/Screenshot.png)

This is an example of a buffer overflow due to the use of MultiByteToWideChar. This function is
used to convert an ANSI string to unicode, and that implies each character expands to two bytes.
Sometimes, a C/C++ programmer miscalculates this.

For example, this is 'A' in hex:

```
41
```

After the conversion, it becomes:

```
0041
```

To exploit this type of bug, we are restricted to only use gadgets that reside in this address
format:

```
00xx00xx
```

For example, if we want a JMP ESP for a unicode overflow, we can't use an address that looks like:

```
0x20417241
```

We could use one that looks like:

```
0x00410072
```

Due to this restriction, unicode without ASLR is already quite challenging to exploit, especially
for a small application. You just may not find enough gadgets to work with. If that is the case,
consider finding ways to load more DLLs, and hopefully there is one that can provide enough gadgets
you can work with.

The [Corelan tutorial](https://www.corelan.be/index.php/2009/11/06/exploit-writing-tutorial-part-7-unicode-from-0x00410041-to-calc/) explains quite well how to exploit this type of bug.
