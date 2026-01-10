#include <Windows.h>
#include <config.h>

CONFIG g_conf;
CONFIG* pg_conf = &g_conf;

SOCKET initialize_connection_provider();
void invoke_dispatch(SOCKET s);
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    if (g_conf.port == NULL)
    {
        return 0;
    }

    auto hConnection = initialize_connection_provider();
    invoke_dispatch(hConnection);
    return 0;
}