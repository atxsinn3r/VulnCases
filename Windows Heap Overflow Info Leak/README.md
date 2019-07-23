# Heap Overflow Information Leak

![Screenshot](https://raw.githubusercontent.com/wchen-r7/VulnCases/master/Windows%20Heap%20Overflow%20Info%20Leak/Screenshot.png)

This demonstrates how to use a heap overflow to leak something in memory on Windows 10.
Unlike Windows 7 and Windows systems before that, exploit devs used to rely on LFH to
create adjacent heap allocations to gain control of an object. In my experiment, about
97% of the time repeated allocations would result a predictable allocation pattern,
but on Windows 10, it is less than 10%, so it's pretty random.

The conditions required for this example include:

* You have control of a heap overflow
* You can create objects the application provides
* You can create heap allocations
* You can use vectors, arrays, and BSTRs
* You can read a string

Typically, a client side application such as a browser engine or actionscript would
satisfy these conditions.

The approach we take to achieve the info leak is this (there are 3 stages):

**First stage : Heap Grooming**

This is what makes Windows 10 more difficult to exploit. Basically you will do a lot
of `!heap -x`, `!heap -p -a`, `!heap -flt s`, `!heap -p -h`, and `dt _HEAP` to figure out
how to lay this out.

1. Create about 16 heap allocations, use an array to track them. The size should be
   carefully selected, ideally you want:
   * It not being in LFH at all.
   * Something that is 4-byte aligned because you'll be working with pointers so
     the alignment is easier to work with.
2. Free one of the allocations. You should consider these conditions before choosing
   which allocation to free:
   * The gap between the allocation you want to free, and the one before that, should
     be close.
   * Same for the allocation after the one you want to free.
3. Create no more than 10 BSTRs, and they should be tracked by an array. The size
   should be smaller than the heap allocations so that they have a chance to fall into
   the hole. I think some people call this condition "coalesced". You want at least one
   BSTR falling into the right one. Notice that the address for the BSTR may not be
   exactly the same as the freed one, but this is expected, because there is an extra
   4-byte header. You should see the freed allocation being busy again anyway.
4. Free another allocation. This one should be right after the BSTR that fell into
   the hole.
5. Create no more than 10 vectors. The size of these vectors should be less than the
   freed allocation, and they should made up of pointers for the same object (that should
   contain a vftable). 
6. At this point, you should have a layout that looks like this:
   `[ Heap Chunk ][ BSTR ][ Objects ]`

**Second stage : Overflowing the heap**

1. Knowing that the BSTR has a four-byte size header, and the content is unicode,
   carefully overflow the chunk, and write past the four-byte-size header. The
   most important mission of the heap overflow is really just overwrite the
   size header of a BSTR.

**Third stage : Reading the vftable**

1. Read the BSTR with the modified size header due to the heap overflow. You will
   probably read a lot of data back, but in there, there should be a four-byte
   value that is basically a pointer to the object.
2. After you find the four bytes, you will need some conversion to convert those
   4 bytes into an integer.
3. When an object is created, usually the first thing that happens is:
   1. A heap allocation
   2. A vftable assigned to it
   Because of that, you may need to dereference.
4. Finally, calculate the offset between the vftable and the image base. And then
   use that to find the image base.


Tested on: Windows 10 as of May 29th, 2019

Special thanks to the following folks:

1. mr_me : https://srcincite.io/
2. Corelanc0d3r : https://www.corelan-training.com/

## Full Writeup

https://blog.rapid7.com/2019/06/12/heap-overflow-exploitation-on-windows-10-explained/