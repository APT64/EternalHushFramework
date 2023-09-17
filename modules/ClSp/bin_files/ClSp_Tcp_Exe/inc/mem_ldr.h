#include <Windows.h>

typedef struct _BASE_RELOCATION_ENTRY {
	WORD Offset : 12;
	WORD Type : 4;
} BASE_RELOCATION_ENTRY;

typedef enum {
	ViewShare = 1,
	ViewUnmap = 2
} SECTION_INHERIT;

typedef NTSTATUS(*NTMAPVIEWOFSECTION)(
	HANDLE          SectionHandle,
	HANDLE          ProcessHandle,
	PVOID* BaseAddress,
	ULONG_PTR       ZeroBits,
	SIZE_T          CommitSize,
	PLARGE_INTEGER  SectionOffset,
	PSIZE_T         ViewSize,
	SECTION_INHERIT InheritDisposition,
	ULONG           AllocationType,
	ULONG           Win32Protect
	);

typedef NTSTATUS(*NTCREATESECTION)(
	PHANDLE SectionHandle,
	ACCESS_MASK DesiredAccess,
	PVOID ObjectAttributes,
	PLARGE_INTEGER MaximumSize,
	ULONG SectionPageProtection,
	ULONG AllocationAttributes,
	HANDLE FileHandle
	);

typedef NTSTATUS(*NTUNMAPVIEWOFSECTION)(
		HANDLE ProcessHandle,
		PVOID  BaseAddress
	);

PULONG _LoadLibrary(char* payload);
void _UnloadLibrary(PVOID baseaddr);
FARPROC _GetProcAddress(HMODULE hModule, LPCSTR lpProcName);