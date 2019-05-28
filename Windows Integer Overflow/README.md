# Windows Integer Overflow

![Screenshot_IDA](https://raw.githubusercontent.com/wchen-r7/VulnCases/master/Windows%20Integer%20Overflow/Screenshot_IDA.png)

This example is an integer overflow. It mimics a type of scenario when an application wants to
parse a file format that contains a size and data field. The size field is one byte, which
covers a value between 0x00 to 0xff. When the size check occurs, it stores the value in a
`char` data type, which by default is a `signed char`, and that only holds a value between
-128 to 127. when the size field is something larger than 127, an integer overflow occurs,
which bypasses the length check, and copies the data into a stack buffer. The result causes
the stack to overflow, and is expected to overwrite an SEH chain.
