#include <pipe_executor.h>
#include <hashtable.h>
#include <syslib.hpp>
#include <intstructs.h>
#include <cutils.h>
extern syslib* psyslib;
BOOL __stdcall SetHandleInformationImpl(HANDLE hObject, DWORD dwMask, DWORD dwFlags)
{
#define ObjectHandleFlagInformation 4
    char v3;
    char v4;
    HANDLE hHandle;
    NTSTATUS status;
    BOOL bResult;
    OBJECT_HANDLE_FLAG_INFORMATION ObjectInformation;
    bool v10;

    v3 = dwFlags;
    v4 = dwMask;
    switch ((DWORD)hObject)
    {
    case 0xFFFFFFF4:
        hHandle = NtCurrentPeb()->ProcessParameters->StandardError;
        break;
    case 0xFFFFFFF5:
        hHandle = NtCurrentPeb()->ProcessParameters->StandardOutput;
        break;
    case 0xFFFFFFF6:
        hHandle = NtCurrentPeb()->ProcessParameters->StandardInput;
        break;
    }
    status = psyslib->nt_call(HASH_NtQueryObject, hHandle, ObjectHandleFlagInformation, &ObjectInformation, sizeof(ObjectInformation), NULL);
    if (status < 0) {
        SetLastError(status);
        bResult = 0;
    }
    bResult = TRUE;
    if ((v4 & 1) != 0)
        ObjectInformation.Inherit = v3 & 1;
    if ((v4 & 2) != 0)
        v10 = (v3 & 2) != 0;
    status = psyslib->nt_call(HASH_NtSetInformationObject, hHandle, ObjectHandleFlagInformation, &ObjectInformation, sizeof(ObjectInformation));
    if (status < 0)
    {
        SetLastError(status);
        bResult = 0;
    }
    return bResult;
}
PROCESS_INFORMATION run_process(std::string cmd, PHANDLE rd, PHANDLE wr, int flags)
{
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    
    HANDLE g_hChildStd_IN_Rd = NULL;
    HANDLE g_hChildStd_IN_Wr = NULL;
    HANDLE g_hChildStd_OUT_Rd = NULL;
    HANDLE g_hChildStd_OUT_Wr = NULL;

    CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0xffffffff);
    CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0xffffffff);
    SetHandleInformationImpl(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformationImpl(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

    *rd = g_hChildStd_OUT_Rd;
    *wr = g_hChildStd_IN_Wr;
    
    return CreateChildProcess(cmd, g_hChildStd_OUT_Wr, g_hChildStd_IN_Rd, flags);
}

PROCESS_INFORMATION CreateChildProcess(std::string szCmdline, HANDLE g_hChildStd_OUT_Wr, HANDLE g_hChildStd_IN_Rd, int flags)
{
    PROCESS_INFORMATION piProcInfo = {0};
    STARTUPINFO siStartInfo = { 0};
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput = g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    HANDLE hProcess = 0, hThread = 0;
    //psyslib->nt_call(HASH_NtCreateUserProcess, &hProcess, &hThread, PROCESS_ALL_ACCESS, THREAD_ALL_ACCESS, 0, 0, CREATE_NO_WINDOW | flags, 0, 0);
    BOOL f = CreateProcessA(NULL,
        (LPSTR)szCmdline.c_str(),
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW | flags,
        NULL,
        NULL,
        (LPSTARTUPINFOA)&siStartInfo,
        &piProcInfo);
   // LARGE_INTEGER to = {5000};
   // psyslib->nt_call_3arg(HASH_NtWaitForSingleObject, piProcInfo.hProcess, FALSE, &to);
    return piProcInfo;
}

std::string ReadFromPipe(HANDLE g_hChildStd_OUT_Rd)
{
    std::string buffer;
    DWORD totalBytesAvail = 0, _totalBytesAvail = 0;

    PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &totalBytesAvail, NULL);
    char* _b = new char[totalBytesAvail];
    _totalBytesAvail = totalBytesAvail;
    DWORD read = 0;
    DWORD totalRead = 0;
    while (totalBytesAvail)
    {
        IO_STATUS_BLOCK io = { 0 };
        psyslib->nt_call(HASH_NtReadFile, g_hChildStd_OUT_Rd, 0, 0, 0, &io, _b, totalBytesAvail - totalRead, 0, 0);
        read = io.Information;
        //ReadFile(g_hChildStd_OUT_Rd, _b, totalBytesAvail - totalRead, &read, NULL);
        totalRead += read;
        buffer += _b;
        _memset(_b, 0,_totalBytesAvail);
        if (!PeekNamedPipe(g_hChildStd_OUT_Rd, NULL, 0, NULL, &totalBytesAvail, NULL)) {
            buffer.resize(totalRead);
            return buffer;
        }
    }
    buffer.resize(totalRead);
    return buffer;
}

ULONG WriteToPipe(HANDLE g_hChildStd_IN_Wr, PUCHAR dataBlob, ULONG dataSz)
{
    DWORD dwWrite;
    IO_STATUS_BLOCK io = {0};
 
    psyslib->nt_call(HASH_NtWriteFile, g_hChildStd_IN_Wr, 0, 0, 0, &io, dataBlob, dataSz, 0, 0);
    dwWrite = io.Information;
    //WriteFile(g_hChildStd_IN_Wr, dataBlob, dataSz, &dwRead, 0);

    return dwWrite;
}