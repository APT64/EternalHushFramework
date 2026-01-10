#pragma once
#include <Windows.h>
#include <intstructs.h>
#include <TlHelp32.h>
typedef struct _SECTION_ALLOC
{
	LONG64 localSection;
	LONG64 remoteSection;
} SECTION_ALLOC, * PSECTION_ALLOC;
typedef enum _SECTION_INHERIT {
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT, * PSECTION_INHERIT;

PSECTION_ALLOC SectionAlloc(HANDLE handle, LONGLONG size, DWORD section_protection, DWORD page_protection, DWORD section_attr);
HANDLE RtlCreateUserThread(HANDLE hProcess, LPVOID lpStartAddress, LPVOID lpParameter, DWORD flags, LPDWORD lpThreadId);
BOOL ProcessStartApcRoutine(HANDLE hProcess, LPVOID lpStartAddress, LPVOID lpParameter);