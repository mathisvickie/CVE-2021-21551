# CVE-2021-21551
Simple PoC for exploiting CVE-2021-21551 for LPE by spawning system cmd.

https://cve.mitre.org/cgi-bin/cvename.cgi?name=CVE-2021-21551

An issue was discovered in signed dell windows driver (dbutil_2_3.sys) which may lead to compromisation of whole local system. Driver's ioctl dispatch routine lacks of validation of user supplied buffer.

## IOCTL
Anyone can create handle and issue ioctl requests to these ioctl codes which break windows security model:

- _0x9b0c1f40_ - arbitrary physical memory read
- _0x9b0c1f44_ - arbitrary physical memory write
- _0x9b0c1ec4_ - arbitrary kernel memory read
- _0x9b0c1ec8_ - arbitrary kernel memory write
- _0x9b0c1ecc_ - controlled parameters to MmFreeContiguousMemorySpecifyCache call
- _0x9b0c1ec0_ - controlled parameters to MmAllocateContiguousMemorySpecifyCache call
- _0x9b0c1f00_ & _0x9b0c1f8c_ & _0x9b0c1f88_ & _0x9b0c1f84_ & _0x9b0c1f80_ - access to some ports

## Compiling PoC
This PoC exploits _0x9b0c1ec4_/_0x9b0c1ec8_ ioctl codes for arbitrary kernel memory read/write respectively. Firstly it locates _PsInitialSystemProcess_ in kernel to get system token and then it writes this token to current process _EPROCESS_ struct. Before compiling update your _EPROCESS_ offsets to avoid BSOD using https://www.vergiliusproject.com/
```c
DWORD EPROCESS_ActiveProcessLinks = 0x2e8;
DWORD EPROCESS_Token = 0x348;
```
Use visual studio to compile (requires windows header). Ntdll SDK header: https://github.com/mathisvickie/segy-software/blob/main/external/ntdll.h

## Running
Tested on:
- Windows 8
- Windows 10 2004
- Windows 10 20H2

Successful exploit will run system cmd. If you get bugcheck _PAGE_FAULT_IN_NONPAGED_AREA_ then the offsets are probably incorrect. Sample output:
![404](https://github.com/mathisvickie/CVE-2021-21551/blob/main/pic.png)
