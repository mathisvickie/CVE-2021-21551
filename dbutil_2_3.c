
NTSTATUS entry(_DRIVER_OBJECT *DriverObject,_UNICODE_STRING *RegistryPath)

{
  void *_Dst;
  NTSTATUS NVar1;
  _DEVICE_OBJECT *local_98;
  _UNICODE_STRING local_90;
  _UNICODE_STRING local_80;
  undefined8 local_70 [5];
  undefined8 local_48 [6];
  ulonglong local_18;
  
  if (((DAT_00013100 == 0) || (DAT_00013100 == 0x2b992ddfa232)) &&
     (DAT_00013100 = (_DAT_fffff78000000320 ^ 0x13100) & 0xffffffffffff, DAT_00013100 == 0)) {
    DAT_00013100 = 0x2b992ddfa232;
  }
  DAT_00013108 = ~DAT_00013100;
  local_18 = DAT_00013100 ^ (ulonglong)&stack0xffffffffffffff28;
  CopyMemoryBlock(local_70,(undefined8 *)L"\\Device\\DBUtil_2_3",0x26);
  CopyMemoryBlock(local_48,(undefined8 *)L"\\DosDevices\\DBUtil_2_3",0x2e);
  RtlInitUnicodeString(&local_90,(wchar_t *)local_70);
  RtlInitUnicodeString(&local_80,(wchar_t *)local_48);
  NVar1 = IoCreateDevice(DriverObject,0xa0,&local_90,0x9b0c,0,'\x01',&local_98);
  if (NVar1 == 0) {
    NVar1 = IoCreateSymbolicLink(&local_80,&local_90);
    if (NVar1 == 0) {
      *(code **)&DriverObject->DispatchShutdown = ioctl;
      *(code **)&DriverObject->DispatchCreate = ioctl;
      *(code **)&DriverObject->DispatchClose = ioctl;
      *(code **)&DriverObject->DispatchDeviceIOControl = ioctl;
      _Dst = local_98->DeviceExtension;
      memset(_Dst,0,0xa0);
      *(undefined8 *)((longlong)_Dst + 0x10) = 0;
      KeInitializeDpc((longlong)_Dst + 0x18,unk_DONOTEXECUTE1,_Dst);
      KeSetTargetProcessorDpc((longlong)_Dst + 0x18,0);
      KeSetImportanceDpc((longlong)_Dst + 0x18,2);
    }
    else {
      IoDeleteDevice(local_98);
    }
  }
  NVar1 = FUN_00011730(local_18 ^ (ulonglong)&stack0xffffffffffffff28);
  return NVar1;
}



undefined8 ArbitraryPhysMemReadWrite(undefined8 *devext,char oneRead_zeroWrite)

{
  undefined8 PhysicalAddress;
  undefined *BaseAddress;
  ulonglong size;
  ulonglong NumberOfBytes;
  undefined *start;
  undefined *sysbuff_plus_10;
  undefined controlled0 [16];
  longlong controlled0_0;
  undefined8 physaddr;
  
  if (*(uint *)(devext + 1) < 0x10) {
    PhysicalAddress = 0xc000000d;
  }
  else {
                    /* WARNING: Load size is inaccurate */
    controlled0 = *(undefined *)(undefined *)*devext;
    if ((devext[2] == 0) || (controlled0_0 = SUB168(controlled0,0), devext[2] == controlled0_0)) {
      NumberOfBytes = (ulonglong)(*(uint *)(devext + 1) - 0x10);
      physaddr = SUB168(controlled0 >> 0x40,0);
      sysbuff_plus_10 = (undefined *)*devext + 0x10;
      PhysicalAddress = FUN_0001155c(physaddr);
      BaseAddress = (undefined *)MmMapIoSpace(PhysicalAddress,NumberOfBytes,MmNonCached);
      if (BaseAddress == (undefined *)0x0) {
        PhysicalAddress = 0xc00000a0;
      }
      else {
        size = NumberOfBytes;
        start = BaseAddress;
        if (oneRead_zeroWrite == '\0') {
          while (size != 0) {
            *start = *sysbuff_plus_10;
            size = size - 1;
            sysbuff_plus_10 = sysbuff_plus_10 + 1;
            start = start + 1;
          }
          LOCK();
        }
        else {
          while (size != 0) {
            *sysbuff_plus_10 = *start;
            size = size - 1;
            start = start + 1;
            sysbuff_plus_10 = sysbuff_plus_10 + 1;
          }
        }
        MmUnmapIoSpace(BaseAddress,NumberOfBytes);
        PhysicalAddress = 0;
                    /* WARNING: Store size is inaccurate */
        *(undefined *)*devext = controlled0;
      }
    }
    else {
      PhysicalAddress = 0xc0000005;
    }
  }
  return PhysicalAddress;
}


undefined8 wrapper_MmAllocateContiguousMemorySpecifyCache(undefined8 *devext)

{
  undefined8 uVar1;
  PHYSICAL_ADDRESS PVar2;
  longlong local_38;
  uint local_30;
  undefined8 local_28;
  undefined8 local_20;
  PVOID local_18;
  undefined8 local_10;
  
  if (*(int *)(devext + 1) == 0x30) {
    CopyMemoryBlock(&local_38,(undefined8 *)*devext,0x30);
    if ((devext[2] == 0) || (devext[2] == local_38)) {
      FUN_0001155c(0);
      FUN_0001155c(local_20);
      uVar1 = FUN_0001155c(local_28);
      local_18 = (PVOID)MmAllocateContiguousMemorySpecifyCache((ulonglong)local_30,uVar1);
      if (local_18 == (PVOID)0x0) {
        uVar1 = 0xc00000a0;
      }
      else {
        PVar2 = MmGetPhysicalAddress(local_18);
        local_10 = FUN_00011574(PVar2);
        CopyMemoryBlock((undefined8 *)*devext,&local_38,0x30);
        uVar1 = 0;
      }
    }
    else {
      uVar1 = 0xc0000005;
    }
  }
  else {
    uVar1 = 0xc000000d;
  }
  return uVar1;
}


undefined8 ArbitraryKrnlMemReadWrite(longlong **devext,char oneRead_zeroWrite)

{
  longlong *ctrl0;
  longlong *ctrl1;
  longlong *ctrl2;
  undefined8 uVar1;
  longlong **controlled;
  longlong **where;
  longlong **what;
  
  if (*(uint *)(devext + 1) < 0x18) {
    uVar1 = 0xc000000d;
  }
  else {
    what = (longlong **)*devext;
    ctrl0 = *what;
    ctrl1 = what[1];
    ctrl2 = what[2];
    if ((devext[2] == (longlong *)0x0) || (devext[2] == ctrl0)) {
      controlled = (longlong **)(((ulonglong)ctrl2 & 0xffffffff) + (longlong)ctrl1);
      if (oneRead_zeroWrite == '\0') {
        what = what + 3;
        where = controlled;
      }
      else {
        where = what + 3;
        what = controlled;
      }
      CopyMemoryBlock(where,what,(ulonglong)(*(uint *)(devext + 1) - 0x18));
      devext = (longlong **)*devext;
      *devext = ctrl0;
      devext[1] = ctrl1;
      devext[2] = ctrl2;
      uVar1 = 0;
    }
    else {
      uVar1 = 0xc0000005;
    }
  }
  return uVar1;
}


NTSTATUS ioctl(_DEVICE_OBJECT *DeviceObject,_IRP *Irp)

{
  int InputBufferLength;
  uint IoControlCode;
  _IO_STACK_LOCATION *CurrentStackLocation;
  longlong **DeviceExtension;
  longlong *sysbuff;
  longlong *controlled;
  int ntstatus;
  undefined8 uVar1;
  longlong *plVar2;
  char oneRead_zeroWrite;
  longlong **what;
  undefined8 uVar3;
  undefined7 uVar4;
  longlong *local_68;
  longlong *controlledbuff [8];
  undefined local_18;
  
  CurrentStackLocation = Irp->CurrentStackLocation;
  DeviceExtension = (longlong **)DeviceObject->DeviceExtension;
  ntstatus = 0;
  *(undefined4 *)(DeviceExtension + 1) = 0;
  if (CurrentStackLocation->MajorFunction != '\x0e') goto LAB_000114bf;
  *DeviceExtension = Irp->SystemBuffer;
  InputBufferLength = *(int *)&CurrentStackLocation->InputBufferLength;
  *(int *)(DeviceExtension + 1) = InputBufferLength;
  if (InputBufferLength != *(int *)&CurrentStackLocation->OutputBufferLength) goto LAB_000111b9;
  IoControlCode = CurrentStackLocation->IoControlCode;
  if (IoControlCode < 0x9b0c1f41) {
    if (IoControlCode == 0x9b0c1f40) {
      oneRead_zeroWrite = '\x01';
physmemwrite:
      uVar1 = ArbitraryPhysMemReadWrite(DeviceExtension,oneRead_zeroWrite);
      ntstatus = (int)uVar1;
    }
    else {
      if (IoControlCode == 0x9b0c1ec0) {
        uVar1 = wrapper_MmAllocateContiguousMemorySpecifyCache(DeviceExtension);
        ntstatus = (int)uVar1;
      }
      else {
        if (IoControlCode == 0x9b0c1ec4) {
          oneRead_zeroWrite = '\x01';
        }
        else {
          if (IoControlCode != 0x9b0c1ec8) {
            if (IoControlCode == 0x9b0c1ecc) {
              if (InputBufferLength == 0x18) {
                what = (longlong **)*DeviceExtension;
                sysbuff = *what;
                local_68 = what[2];
                if ((DeviceExtension[2] == (longlong *)0x0) || (DeviceExtension[2] == sysbuff)) {
                  MmFreeContiguousMemorySpecifyCache(what[1],(ulonglong)local_68 & 0xffffffff,0);
                  local_68 = (longlong *)((ulonglong)local_68 & 0xffffffff00000000);
                  what = (longlong **)*DeviceExtension;
                  *what = sysbuff;
                  what[1] = (longlong *)0x0;
                  what[2] = local_68;
                  goto LAB_000114bf;
                }
                goto LAB_00011248;
              }
            }
            else {
              if (IoControlCode == 0x9b0c1f00) {
                if (InputBufferLength == 0x48) {
                  CopyMemoryBlock(controlledbuff,*DeviceExtension,0x48);
                  if ((DeviceExtension[2] == (longlong *)0x0) ||
                     (DeviceExtension[2] == controlledbuff[0])) {
                    CopyMemoryBlock(DeviceExtension + 0xb,controlledbuff,0x48);
                    unk_DONOTEXECUTE1(0,DeviceExtension,0,0);
                    CopyMemoryBlock(controlledbuff,DeviceExtension + 0xb,0x48);
LAB_000112bb:
                    what = controlledbuff;
LAB_00011256:
                    CopyMemoryBlock(*DeviceExtension,what,0x48);
                    goto LAB_000114bf;
                  }
                  goto LAB_00011248;
                }
              }
              else {
                if (IoControlCode == 0x9b0c1f04) {
                  if (InputBufferLength == 0x48) {
                    CopyMemoryBlock(controlledbuff,*DeviceExtension,0x48);
                    if ((DeviceExtension[2] == (longlong *)0x0) ||
                       (DeviceExtension[2] == controlledbuff[0])) {
                      local_18 = 0;
                      CopyMemoryBlock(DeviceExtension + 0xb,controlledbuff,0x48);
                      KeInsertQueueDpc(DeviceExtension + 3,DeviceExtension,DeviceExtension);
                      goto LAB_000112bb;
                    }
                    goto LAB_00011248;
                  }
                }
                else {
                  if ((IoControlCode == 0x9b0c1f08) && (InputBufferLength == 0x48)) {
                    CopyMemoryBlock(controlledbuff,*DeviceExtension,0x48);
                    if ((DeviceExtension[2] == (longlong *)0x0) ||
                       (DeviceExtension[2] == controlledbuff[0])) {
                      what = DeviceExtension + 0xb;
                      goto LAB_00011256;
                    }
LAB_00011248:
                    ntstatus = -0x3ffffffb;
                    goto LAB_000114c6;
                  }
                }
              }
            }
            goto LAB_000111b9;
          }
          oneRead_zeroWrite = '\0';
        }
        uVar1 = ArbitraryKrnlMemReadWrite(DeviceExtension,oneRead_zeroWrite);
        ntstatus = (int)uVar1;
      }
    }
LAB_000114b9:
    if (ntstatus != 0) goto LAB_000114c6;
  }
  else {
    if (IoControlCode == 0x9b0c1f44) {
      oneRead_zeroWrite = '\0';
      goto physmemwrite;
    }
    uVar4 = (undefined7)((ulonglong)CurrentStackLocation >> 8);
    if (IoControlCode == 0x9b0c1f80) {
      uVar1 = CONCAT71(uVar4,1);
noexecute1:
      uVar3 = 0;
noexecute0:
      ntstatus = unk_NOEXECUTE0(DeviceExtension,uVar3,uVar1);
      goto LAB_000114b9;
    }
    if (IoControlCode == 0x9b0c1f84) {
      uVar1 = 0;
      goto noexecute1;
    }
    if (IoControlCode == 0x9b0c1f88) {
      uVar1 = CONCAT71(uVar4,1);
      uVar3 = 0x9b0c1f01;
      goto noexecute0;
    }
    if (IoControlCode == 0x9b0c1f8c) {
      uVar1 = 0;
      uVar3 = 0x9b0c1f01;
      goto noexecute0;
    }
    ntstatus = 0;
    if (IoControlCode == 0x9b0c1fc0) {
      if (InputBufferLength == 0xc) {
        sysbuff = *DeviceExtension;
        controlled = DeviceExtension[2];
        *sysbuff = 0x300000002;
        *(uint *)(sysbuff + 1) =
             *(uint *)(sysbuff + 1) & 0xffffff00 | (uint)(controlled != (longlong *)0x0);
        goto LAB_000114bf;
      }
LAB_000111b9:
      ntstatus = -0x3ffffff3;
      goto LAB_000114c6;
    }
    if ((IoControlCode != 0x9b0c1fc4) || (InputBufferLength != 8)) goto LAB_000111b9;
    sysbuff = DeviceExtension[2];
    controlled = (longlong *)**DeviceExtension;
    if ((sysbuff != (longlong *)0x0) && (sysbuff != controlled)) goto LAB_00011248;
    plVar2 = (longlong *)0x0;
    if (sysbuff == (longlong *)0x0) {
      plVar2 = controlled;
    }
    DeviceExtension[2] = plVar2;
    *(longlong **)*DeviceExtension = controlled;
  }
LAB_000114bf:
  (Irp->IoStatus).Information = (ulonglong)*(uint *)(DeviceExtension + 1);
LAB_000114c6:
  (Irp->IoStatus).Status = ntstatus;
  IofCompleteRequest(Irp,'\0');
  return (NTSTATUS)ntstatus;
}
