#include <iostream>
#include <windows.h>
#include "argparser.h"

ULONG GetFile(PCHAR& dataBlob, HANDLE hFile)
{
    BOOL bErrorFlag = FALSE;
    DWORD dwBytesReaded = 0;

    ULONG dataSz = GetFileSize(hFile, 0);

    dataBlob = new char[dataSz];

    bErrorFlag = ReadFile(
        hFile,
        dataBlob,
        dataSz,
        &dwBytesReaded,
        NULL);
    if (!bErrorFlag)
    {
        return -1;
    }
    return dwBytesReaded;
}
struct MyOpts
{
    string mode{};
    string in{};
    string _export{};
    int ordinal{-1};
};

auto parser = CmdOpts<MyOpts>::Create({
    {"--mode", &MyOpts::mode },
    {"--in", &MyOpts::in },
    {"--export", &MyOpts::_export },
    {"--ordinal", &MyOpts::ordinal }
    });

void alertable_routine() {
    while (true)
    {
        SleepEx(INFINITE, 1);
    }
}
void thread_sleep_mode() {
    HANDLE hTimer = CreateWaitableTimerA(0, true, 0);
    WaitForSingleObject(hTimer, INFINITE);
}

void alertable_thread_mode() {
    WaitForSingleObject(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)alertable_routine, 0, 0, 0), INFINITE);
}
void execute_shellcode(string in_path) {
    HANDLE hFile = CreateFileA(in_path.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    char* file_buf;
    DWORD size = GetFile(file_buf, hFile);
    if (size)
    {
        LPVOID addr = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if (addr)
        {
            memcpy(addr, file_buf, size);
            HANDLE hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)addr, 0, 0, 0);
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }
    }
}
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    BCRYPT_KEY_HANDLE hKey = 0;
    unsigned char* publicKey = 0;
    unsigned char* privateKey = 0;

    char** argv;
    int argc;

    get_command_line_args(&argc, &argv);

    auto args = parser->parse(argc, (const char**)argv);
    if (args.mode == "thread_sleep")
    {
        thread_sleep_mode();
    }
    if (args.mode == "alertable_thread")
    {
        alertable_thread_mode();
    }
    if (args.mode == "shellcode" && args.in != "")
    {
        execute_shellcode(args.in);
    }
    if (args.mode == "dll" && args.in != "")
    {
        HMODULE hlib = LoadLibraryA(args.in.c_str());
        if (args._export != "")
        {
            auto proc =  GetProcAddress(hlib, args._export.c_str());
            proc();
        }else if(args.ordinal != (-1)){
            auto proc = GetProcAddress(hlib, MAKEINTRESOURCE(args.ordinal));
            proc();
        }
        
    }
}