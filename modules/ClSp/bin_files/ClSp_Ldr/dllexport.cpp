#include <Windows.h>
#include <config.h>

CONFIG g_conf;
CONFIG* pg_conf = &g_conf;

SOCKET initialize_connection_provider();
void invoke_dispatch(SOCKET s);

void startup() {
    if (g_conf.port == NULL) return;
    auto hConnection = initialize_connection_provider();
    invoke_dispatch(hConnection);
}


extern "C" __declspec(dllexport) HRESULT DllRegisterServer()
{
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)startup, NULL, NULL, NULL);
    return S_OK;
}

__declspec(dllexport) BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        if (g_conf.dll_attached_run) DllRegisterServer();
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr)
        {
            break;
        }
        break;
    }
    return TRUE;
}