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
            if (status == 0xc0000004)
            {
                GlobalFree(pHandleInfo);
                pHandleInfo = 0;
            }
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
        if (*object == 0) status = ERROR_MOD_NOT_FOUND;
        if (pHandleInfo) GlobalFree(pHandleInfo);
        return status;
	}

    NTSTATUS LeakModuleBase(std::string modname, PVOID64* base) {
        ULONG len = 20;
        NTSTATUS status = (NTSTATUS)0xc0000004;
        PSYSTEM_MODULE_INFORMATION pModuleInfo = NULL;
        do {
            len *= 2;
            pModuleInfo = (PSYSTEM_MODULE_INFORMATION)GlobalAlloc(GMEM_ZEROINIT, len);

            status = pCtx->psyslib->nt_call(HASH_NtQuerySystemInformation, SystemModuleInformation, pModuleInfo, len, &len);
            if (status == 0xc0000004)
            {
                GlobalFree(pModuleInfo);
                pModuleInfo = 0;
            }

        } while (status == (NTSTATUS)0xc0000004);
        if (!NT_SUCCESS(status)) {
            if (pModuleInfo) GlobalFree(pModuleInfo);
            return status;
        }
        for (int i = 0; i < pModuleInfo->ModulesCount; i++) {
            PCHAR name = pModuleInfo->Modules[i].Name;
            if (pModuleInfo->Modules[i].ImageBaseAddress == 0) status = ERROR_ACCESS_DENIED;
            if (strcmp(name, modname.c_str()) == 0)
            {
                status = ERROR_SUCCESS;
                *base = pModuleInfo->Modules[i].ImageBaseAddress;
                break;
            }
        }
        if (*base == 0) status = ERROR_MOD_NOT_FOUND;
        if (pModuleInfo) GlobalFree(pModuleInfo);
        return status;
    }
};

typedef struct {
	LONGLONG layer_uid;
	DaNaBaseClass* layer_object;
}LAYER_ENTRY;