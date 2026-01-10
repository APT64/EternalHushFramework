#include <Windows.h>
#include <vector>
#include <net.h>
#include <iostream>

#pragma pack(1)
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
	USHORT platform_arch;
	USHORT implant_platform;
	USHORT nt_major;
	USHORT nt_minor;
	USHORT buildno_major;
	USHORT buildno_minor;
} IMPLANTINFO, * PIMPLANTINFO;

typedef struct _ACTIONINFO {
	USHORT rcall_code;
	ULONG data_size;
	char padding[10];
} ACTIONINFO, * PACTIONINFO;

typedef struct _HELLORESPONSE {
	char implant_info[256];
	char next_iv[16];
} HELLORESPONSE, * PHELLORESPONSE;

typedef struct _EXTINFO {
	ULONG payload_size;
	char hash[12];
} EXTENSIONINFO, * PEXTENSIONINFO;

typedef struct _PAYLOADINFO {
	ULONG payload_size;
	USHORT payload_type;
	USHORT payload_arch;
	USHORT payload_argc;
	char padding[6];
} PAYLOADINFO, * PPAYLOADINFO;

typedef struct _PAYLOAD {
	USHORT part_size;
	char payload_part[4094];
} PAYLOAD, * PPAYLOAD;
#pragma pack(0)