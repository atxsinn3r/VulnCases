# Windows Out of Scope Object Double Free

A classic example of a double free goes like this:

```cpp
void* buf = malloc(10);
free(buf);
// ... some code ...
// ... so much code maybe you've lost track of the buf ...
free(buf);
```

However, a free-related bug isn't necessarily written explicitly that way. A scenario you may more likely run into is when an object stored on the stack goes out of scope, which triggers the destructor to kick in. If this behavior is overlooked and the destructor happens to free something, a double-free condition could occur.

For example, in the DoubleFree.cpp file, the destructor is actually called twice. Once when the Test function exits, another in the main function. The result in WinDBG with gflags enabled would look like this:

```
0:000> g


===========================================================
VERIFIER STOP 00000007: pid 0x1974: block already freed 

	007A1000 : Heap handle
	076E230C : Heap block
	00000400 : Block size
	00000000 : 
===========================================================
This verifier stop is not continuable. Process will be terminated 
when you use the `go' debugger command.
===========================================================

(1974.1f88): Break instruction exception - code 80000003 (first chance)
eax=00563000 ebx=00000000 ecx=00000001 edx=006ff64c esi=6b57acd0 edi=00000000
eip=6b57dd42 esp=006ff5ec ebp=006ff5f4 iopl=0         nv up ei pl nz na po nc
cs=0023  ss=002b  ds=002b  es=002b  fs=0053  gs=002b             efl=00000202
verifier!VerifierBreakin+0x42:
6b57dd42 cc              int     3
0:000> !analyze -v
*******************************************************************************
*                                                                             *
*                        Exception Analysis                                   *
*                                                                             *
*******************************************************************************

*** WARNING: Unable to verify checksum for image009e0000
APPLICATION_VERIFIER_HEAPS_DOUBLE_FREE (7)
Heap block already freed.
This situation happens if the block is freed twice. Freed blocks are marked in a
special way and are kept around for a while in a delayed free queue. If a buggy
program tries to free the block again this will be caught assuming the block was not
dequeued from delayed free queue and its memory reused for other allocations.
The depth of the delay free queue is in the order of thousands of blocks therefore
there are good chances that most double frees will be caught. 
Arguments:
Arg1: 007a1000, Heap handle for the heap owning the block. 
Arg2: 076e230c, Heap block being freed again. 
Arg3: 00000400, Size of the heap block. 
Arg4: 00000000, Not used 

KEY_VALUES_STRING: 1

    Key  : Timeline.OS.Boot.DeltaSec
    Value: 1895

    Key  : Timeline.Process.Start.DeltaSec
    Value: 13


PROCESSES_ANALYSIS: 1

SERVICE_ANALYSIS: 1

STACKHASH_ANALYSIS: 1

TIMELINE_ANALYSIS: 1

Timeline: !analyze.Start
    Name: <blank>
    Time: 2019-08-24T00:55:47.915Z
    Diff: 84 mSec

Timeline: Dump.Current
    Name: <blank>
    Time: 2019-08-24T00:55:48.0Z
    Diff: 0 mSec

Timeline: Process.Start
    Name: <blank>
    Time: 2019-08-24T00:55:35.0Z
    Diff: 13000 mSec

Timeline: OS.Boot
    Name: <blank>
    Time: 2019-08-24T00:24:13.0Z
    Diff: 1895000 mSec


DUMP_CLASS: 2

DUMP_QUALIFIER: 0

FAULTING_IP: 
verifier!VerifierBreakin+42
6b57dd42 cc              int     3

EXCEPTION_RECORD:  (.exr -1)
ExceptionAddress: 6b57dd42 (verifier!VerifierBreakin+0x00000042)
   ExceptionCode: 80000003 (Break instruction exception)
  ExceptionFlags: 00000000
NumberParameters: 1
   Parameter[0]: 00000000

FAULTING_THREAD:  00001f88

DEFAULT_BUCKET_ID:  BREAKPOINT_AVRF

PROCESS_NAME:  image009e0000

ERROR_CODE: (NTSTATUS) 0x80000003 - {EXCEPTION}  Breakpoint  A breakpoint has been reached.

EXCEPTION_CODE: (HRESULT) 0x80000003 (2147483651) - One or more arguments are invalid

EXCEPTION_CODE_STR:  80000003

EXCEPTION_PARAMETER1:  00000000

WATSON_BKT_PROCSTAMP:  5d6087a9

WATSON_BKT_MODULE:  verifier.dll

WATSON_BKT_MODSTAMP:  603bdbda

WATSON_BKT_MODOFFSET:  dd42

WATSON_BKT_MODVER:  10.0.17763.1

MODULE_VER_PRODUCT:  Microsoft® Windows® Operating System

BUILD_VERSION_STRING:  17763.1.x86fre.rs5_release.180914-1434

MODLIST_WITH_TSCHKSUM_HASH:  a362da52de18297dbbba2706063c644d81070821

MODLIST_SHA1_HASH:  6f78262418e23580b631549639de6b67d9943de7

NTGLOBALFLAG:  2000000

PROCESS_BAM_CURRENT_THROTTLED: 0

PROCESS_BAM_PREVIOUS_THROTTLED: 0

APPLICATION_VERIFIER_FLAGS:  0

PRODUCT_TYPE:  1

SUITE_MASK:  272

DUMP_TYPE:  fe

APPLICATION_VERIFIER_LOADED: 1

ANALYSIS_SESSION_HOST:  DESKTOP-20R6DE5

ANALYSIS_SESSION_TIME:  08-23-2019 19:55:47.0915

ANALYSIS_VERSION: 10.0.18362.1 x86fre

THREAD_ATTRIBUTES: 
OS_LOCALE:  ENU

BUGCHECK_STR:  BREAKPOINT_AVRF

PRIMARY_PROBLEM_CLASS:  BREAKPOINT

PROBLEM_CLASSES: 

    ID:     [0n321]
    Type:   [@APPLICATION_FAULT_STRING]
    Class:  Primary
    Scope:  DEFAULT_BUCKET_ID (Failure Bucket ID prefix)
            BUCKET_ID
    Name:   Omit
    Data:   Add
            String: [BREAKPOINT]
    PID:    [Unspecified]
    TID:    [Unspecified]
    Frame:  [0]

    ID:     [0n98]
    Type:   [AVRF]
    Class:  Addendum
    Scope:  DEFAULT_BUCKET_ID (Failure Bucket ID prefix)
            BUCKET_ID
    Name:   Add
    Data:   Omit
    PID:    [0x1974]
    TID:    [0x1f88]
    Frame:  [0] : verifier!VerifierBreakin

LAST_CONTROL_TRANSFER:  from 6b57de40 to 6b57dd42

STACK_TEXT:  
006ff5f4 6b57de40 c0000421 00000000 00000000 verifier!VerifierBreakin+0x42
006ff91c 6b57e13d 00000007 007a1000 076e230c verifier!VerifierCaptureContextAndReportStop+0xf0
006ff960 6b57bb1f 00000007 6b571cbc 007a1000 verifier!VerifierStopMessage+0x2bd
006ff9cc 6b5789ad 007a1000 00000000 076e230c verifier!AVrfpDphReportCorruptedBlock+0x1cf
006ffa30 6b578b05 007a1000 0780ec00 00000000 verifier!AVrfpDphFindBusyMemoryNoCheck+0x7d
006ffa54 6b578d70 007a1000 0780ec00 006ffae4 verifier!AVrfpDphFindBusyMemory+0x15
006ffa70 6b57ad60 007a1000 0780ec00 01000002 verifier!AVrfpDphFindBusyMemoryAndRemoveFromBusyList+0x20
006ffa8c 772bb609 007a0000 01000002 0780ec00 verifier!AVrfDebugPageHeapFree+0x90
006ffaf4 77263452 0780ec00 5b706461 00000000 ntdll!RtlDebugFreeHeap+0x3e
006ffc48 772150c1 00000000 0780ec00 077aaf48 ntdll!RtlpFreeHeap+0x4dff2
006ffc9c 009e7417 007a0000 00000000 0780ec00 ntdll!RtlFreeHeap+0x201
WARNING: Stack unwind information not available. Following frames may be wrong.
006ffcb0 009e5b17 0780ec00 00000000 006ffce0 image009e0000+0x7417
006ffcc0 009e113e 0780ec00 dbf4cbc6 006ffcf0 image009e0000+0x5b17
006ffce0 009e10b6 dbf4ca26 00000000 0780ec00 image009e0000+0x113e
006ffd00 009e13aa 00000001 077a4fd0 077aaf48 image009e0000+0x10b6
006ffd48 77100419 00563000 77100400 006ffdb4 image009e0000+0x13aa
006ffd58 7723662d 00563000 5b70659d 00000000 KERNEL32!BaseThreadInitThunk+0x19
006ffdb4 772365fd ffffffff 772551d8 00000000 ntdll!__RtlUserThreadStart+0x2f
006ffdc4 00000000 009e1432 00563000 00000000 ntdll!_RtlUserThreadStart+0x1b


STACK_COMMAND:  ~0s ; .cxr ; kb

THREAD_SHA1_HASH_MOD_FUNC:  7d2668b2829674ff0253d920f3d46b5aece4985e

THREAD_SHA1_HASH_MOD_FUNC_OFFSET:  3f91cffe1f2c864f7eeabaee79bcc629b8b1c6ec

THREAD_SHA1_HASH_MOD:  2f5e71c10ebac913143da3896ee2277035249edd

FOLLOWUP_IP: 
image009e0000+7417
009e7417 85c0            test    eax,eax

FAULT_INSTR_CODE:  1875c085

SYMBOL_STACK_INDEX:  b

SYMBOL_NAME:  image009e0000+7417

FOLLOWUP_NAME:  MachineOwner

MODULE_NAME: image009e0000

IMAGE_NAME:  image009e0000

DEBUG_FLR_IMAGE_TIMESTAMP:  5d6087a9

BUCKET_ID:  BREAKPOINT_AVRF_image009e0000+7417

FAILURE_EXCEPTION_CODE:  80000003

FAILURE_IMAGE_NAME:  image009e0000

BUCKET_ID_IMAGE_STR:  image009e0000

FAILURE_MODULE_NAME:  image009e0000

BUCKET_ID_MODULE_STR:  image009e0000

FAILURE_FUNCTION_NAME:  Unknown

BUCKET_ID_FUNCTION_STR:  Unknown

BUCKET_ID_OFFSET:  7417

BUCKET_ID_MODTIMEDATESTAMP:  5d6087a9

BUCKET_ID_MODCHECKSUM:  0

BUCKET_ID_MODVER_STR:  0.0.0.0

BUCKET_ID_PREFIX_STR:  BREAKPOINT_AVRF_

FAILURE_PROBLEM_CLASS:  BREAKPOINT

FAILURE_SYMBOL_NAME:  image009e0000!Unknown

FAILURE_BUCKET_ID:  BREAKPOINT_AVRF_80000003_image009e0000!Unknown

TARGET_TIME:  2019-08-24T00:55:51.000Z

OSBUILD:  17763

OSSERVICEPACK:  475

SERVICEPACK_NUMBER: 0

OS_REVISION: 0

OSPLATFORM_TYPE:  x86

OSNAME:  Windows 10

OSEDITION:  Windows 10 WinNt SingleUserTS

USER_LCID:  0

OSBUILD_TIMESTAMP:  unknown_date

BUILDDATESTAMP_STR:  180914-1434

BUILDLAB_STR:  rs5_release

BUILDOSVER_STR:  10.0.17763.1.x86fre.rs5_release.180914-1434

ANALYSIS_SESSION_ELAPSED_TIME:  cb0

ANALYSIS_SOURCE:  UM

FAILURE_ID_HASH_STRING:  um:breakpoint_avrf_80000003_image009e0000!unknown

FAILURE_ID_HASH:  {d3f728e1-6a74-3cb2-714a-f2180f2ee172}

Followup:     MachineOwner
---------

```

