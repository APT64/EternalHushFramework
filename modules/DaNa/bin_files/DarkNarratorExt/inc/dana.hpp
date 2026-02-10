#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <intstructs.h>
#include <superfetch.h>
extern PMODULE_CONTEXT pCtx;

class DaNaBaseClass {
private:
	std::wstring device_name;
	HANDLE device_handle;
public:
	virtual NTSTATUS Initialize() = 0;
	virtual NTSTATUS Finalize() = 0;

	virtual NTSTATUS ReadVirtualMemory(PVOID64 memptr, PVOID64 out_buffer, LONG64 size) = 0;
	virtual NTSTATUS WriteVirtualMemory(PVOID64 input_buffer, PVOID64 memptr, LONG64 size) = 0;

	virtual NTSTATUS ReadPhysicalMemory(PVOID64 memptr, PVOID64 out_buffer, LONG64 size) = 0;
	virtual NTSTATUS WritePhysicalMemory(PVOID64 input_buffer, PVOID64 memptr, LONG64 size) = 0;

	NTSTATUS LeakObjectAddress(HANDLE handle, DWORD pid, PVOID64* object) {
        ULONG len = 20;
        NTSTATUS status = (NTSTATUS)0xc0000004;
        PSYSTEM_HANDLE_INFORMATION_EX pHandleInfo = NULL;
        do {
            len *= 2;
            pHandleInfo = (PSYSTEM_HANDLE_INFORMATION_EX)GlobalAlloc(GMEM_ZEROINIT, len);

            status = pCtx->psyslib->nt_call(HASH_NtQuerySystemInformation, SystemExtendedHandleInformation, pHandleInfo, len, &len);

        } while (status == (NTSTATUS)0xc0000004);
        if (!NT_SUCCESS(status)) {
            if (pHandleInfo) GlobalFree(pHandleInfo);
            return status;
        }
        for (int i = 0; i < pHandleInfo->HandleCount; i++) {
            PVOID _object = pHandleInfo->Handles[i].Object;
            HANDLE _handle = pHandleInfo->Handles[i].HandleValue;
            DWORD _pid = (DWORD)pHandleInfo->Handles[i].UniqueProcessId;
            if (_handle == handle && _pid == pid) {
                status = ERROR_SUCCESS;
                *object = _object;
                if (_object == NULL) status = ERROR_ACCESS_DENIED;
            }
        }
        if (pHandleInfo) GlobalFree(pHandleInfo);
        return status;
	}
};

typedef struct {
	LONGLONG layer_uid;
	DaNaBaseClass* layer_object;
}LAYER_ENTRY;