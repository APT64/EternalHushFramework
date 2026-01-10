#include <Windows.h>
#include <iostream>

PROCESS_INFORMATION CreateChildProcess(std::string szCmdline, HANDLE g_hChildStd_OUT_Wr, HANDLE g_hChildStd_IN_Rd, int flags);
std::string ReadFromPipe(HANDLE g_hChildStd_OUT_Rd);
ULONG WriteToPipe(HANDLE g_hChildStd_IN_Wr, PUCHAR dataBlob, ULONG dataSz);
PROCESS_INFORMATION run_process(std::string cmd, PHANDLE rd, PHANDLE wr, int flags);