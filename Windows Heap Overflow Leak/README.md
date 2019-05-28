# Windows Heap Overflow Leak

This is an example of using a heap overflow that leaks data in memory. While experimental, the
attempt here is to create chunks hopefully like this:

```
[chunk][BSTR]
```

On Windows 10, this is quite difficult to execute because these adjacent chunks are kind of
scattered in memory. There isn't a very reliable way to predict exactly which location would
find you two adjacent chunks, but if you track the chunks in an array, very likely one of them
has the one you want.

What the PoC does here is it overflows every chunk, and then hopefully it overwrites the size
field in the BSTR header. We don't really know for sure which one is succesfully overwritten,
but since they are tracked in an array, eventually we find one that works by determining the
BSTR size (for example: we expect the normla string size is 8, and the corrupt one is isn't),
and we assume that is the BSTR we want. When we read that, it should read past the BSTR,
and give us extra data in memory.

Note: The technique is more specific to Windows 10. It is experiemntal and needs to be
improved.

Notes:

* Instead of using LFH, avoid that on Windows 10. Older Windows work better w/ LFH though.
* Although an array seems to be a nice approach to track chunks, the chunks obviously won't
  be adjacent due to the array. Maybe an array can help improve reliabily, but maybe not
  required at all.