#include <windows.h>
#include <winhttp.h>
#include <TlHelp32.h>

typedef HANDLE(NTAPI* pfnCreateToolhelp32Snapshot)(DWORD, DWORD);
typedef BOOL(NTAPI* pfnThread32First)(HANDLE, LPTHREADENTRY32);
typedef BOOL(NTAPI* pfnThread32Next)(HANDLE, LPTHREADENTRY32);

typedef BOOL(NTAPI* pfnWinHttpCloseHandle)(HINTERNET h);
typedef BOOL(NTAPI* pfnWinHttpReceiveResponse)(HINTERNET h, LPVOID lp);
typedef BOOL(NTAPI* pfnWinHttpWriteData)(HINTERNET hRequest, LPCVOID   lpBuffer, DWORD     dwNumberOfBytesToWrite, LPDWORD   lpdwNumberOfBytesWritten);
typedef BOOL(NTAPI* pfnWinHttpSetTimeouts)(HINTERNET h, int t0, int t1, int t2, int t3);
typedef BOOL(NTAPI* pfnWinHttpSendRequest)(HINTERNET, LPCWSTR, DWORD, LPVOID, DWORD, DWORD, DWORD_PTR);
typedef HINTERNET(NTAPI* pfnWinHttpOpenRequest)(HINTERNET, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR*, DWORD);
typedef BOOL(NTAPI* pfnWinHttpReadData)(HINTERNET, LPVOID, DWORD, LPDWORD);
typedef BOOL(NTAPI* pfnWinHttpQueryDataAvailable)(HINTERNET, LPDWORD);
typedef BOOL(NTAPI* pfnWinHttpQueryHeaders)(HINTERNET, DWORD, LPCWSTR, LPVOID, LPDWORD, LPDWORD);
typedef HINTERNET(NTAPI* pfnWinHttpOpen)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
typedef HINTERNET(NTAPI* pfnWinHttpConnect)(HINTERNET, LPCWSTR, INTERNET_PORT, DWORD);
typedef BOOL(NTAPI* pfnWinHttpSetOption)(HINTERNET, DWORD, LPVOID, DWORD);
typedef BOOL(NTAPI* pfnWinHttpAddRequestHeaders)(HINTERNET, LPCWSTR, DWORD, DWORD);
