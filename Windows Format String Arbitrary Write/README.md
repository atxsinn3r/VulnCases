# Windows Format String Arbitrary Write

![Screenshot](https://raw.githubusercontent.com/wchen-r7/VulnCases/master/Windows%20Format%20String%20Arbitrary%20Write/Screenshot.png)

This is an example of a format string vulnerability for Windows. The code is written in a way
that allows you to easily overwrite an object's state. Technically, you can use this example
to do other format string attacks too (such as reading a pointer on the stack), too.

Note: Make sure you disable GS when you compile, otherwise you can only read stuff from the
stack, not write.
