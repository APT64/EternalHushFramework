#include <Windows.h>
#pragma pack(1)
typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, * PUNICODE_STRING;

typedef struct _PEB_LDR_DATA {
	BYTE       Reserved1[8];
	PVOID      Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE           Reserved1[16];
	PVOID          Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, * PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
	BYTE                          Reserved1[2];
	BYTE                          BeingDebugged;
	BYTE                          Reserved2[1];
	PVOID                         Reserved3[2];
	PPEB_LDR_DATA                 Ldr;
	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
	PVOID                         Reserved4[3];
	PVOID                         AtlThunkSListPtr;
	PVOID                         Reserved5;
	ULONG                         Reserved6;
	PVOID                         Reserved7;
	ULONG                         Reserved8;
	ULONG                         AtlThunkSListPtr32;
	PVOID                         Reserved9[45];
	BYTE                          Reserved10[96];
	PVOID						  PostProcessInitRoutine;
	BYTE                          Reserved11[128];
	PVOID                         Reserved12[1];
	ULONG                         SessionId;
} PEB, * PPEB;

typedef struct _HELLOREQUEST {
	ULONG val1;
	ULONG val2;
	char inital_data[248];
} HELLOREQUEST, * PHELLOREQUEST;

typedef struct _IMPLANTINFO {
	ULONG implant_version;
	ULONG implant_id;
	char session_key[32];
	USHORT implant_arch;
	USHORT implant_platform;
} IMPLANTINFO, * PIMPLANTINFO;

typedef struct _HELLORESPONSE {
	char implant_info[256];
	char next_iv[16];
} HELLORESPONSE, * PHELLORESPONSE;

typedef struct _CONFIG {
	long long magic_value = 0x99c72f6099c72f60;
	short port = 0x0;
	long id = 0x0;
} CONFIG, * PCONFIG;

#pragma pack(0)