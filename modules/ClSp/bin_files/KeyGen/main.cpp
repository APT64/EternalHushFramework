#include <main.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <locale>
#include <codecvt>
#pragma comment (lib, "bcrypt.lib")

#define NT_SUCCESS(Status)          (((NTSTATUS)(Status)) >= 0)

NTSTATUS SaveFile(PUCHAR dataBlob, ULONG dataSz, LPCWSTR filePath)
{
    BOOL bErrorFlag = FALSE;
    DWORD dwBytesWritten = 0;
    HANDLE hFile = NULL;

    hFile = CreateFileW(filePath,
        GENERIC_WRITE,
        0,
        NULL,                   // default security
        OPEN_ALWAYS,             // create new file only
        FILE_ATTRIBUTE_NORMAL,  // normal file
        NULL);                  // no attr. template

    bErrorFlag = WriteFile(
        hFile,           // open file handle
        dataBlob,      // start of data to write
        dataSz,  // number of bytes to write
        &dwBytesWritten, // number of bytes that were written
        NULL);            // no overlapped structure

    CloseHandle(hFile);
    return 0;
}

NTSTATUS export_privkey(BCRYPT_KEY_HANDLE hKey, PUCHAR key_data, ULONG key_size, std::wstring path)
{
    NTSTATUS status;
    ULONG size;

    status = BCryptExportKey(hKey, NULL, BCRYPT_RSAFULLPRIVATE_BLOB, NULL, NULL, &size, 0);
    if (!NT_SUCCESS(status))
    {
        return status;
    }
    key_data = (PUCHAR)malloc(size);
    status = BCryptExportKey(hKey, NULL, BCRYPT_RSAFULLPRIVATE_BLOB, key_data, size, &size, 0);
    if (!NT_SUCCESS(status))
    {
        return status;
    }
    SaveFile(key_data, size, path.c_str());
    key_size = size;
    return status;
}

NTSTATUS export_pubkey(BCRYPT_KEY_HANDLE hKey, PUCHAR key_data, ULONG key_size, std::wstring path)
{
    NTSTATUS status;
    ULONG size;

    status = BCryptExportKey(hKey, NULL, BCRYPT_RSAPUBLIC_BLOB, NULL, NULL, &size, 0);
    if (!NT_SUCCESS(status))
    {
        return status;
    }
    key_data = (PUCHAR)malloc(size);
    status = BCryptExportKey(hKey, NULL, BCRYPT_RSAPUBLIC_BLOB, key_data, size, &size, 0);
    if (!NT_SUCCESS(status))
    {
        return status;
    }
    SaveFile(key_data, size, path.c_str());
    key_size = size;
    return status;
}
NTSTATUS key_gen(BCRYPT_KEY_HANDLE* hKey, int keySize)
{
    BCRYPT_ALG_HANDLE hRsaAlg;
    NTSTATUS status;
    BCryptOpenAlgorithmProvider(&hRsaAlg, BCRYPT_RSA_ALGORITHM, NULL, 0);
    status = BCryptGenerateKeyPair(hRsaAlg, hKey, keySize, 0);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = BCryptFinalizeKeyPair(*hKey, 0);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    return status;
}


int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow) {
    BCRYPT_KEY_HANDLE hKey = 0;
    unsigned char* publicKey = 0;
    unsigned char* privateKey = 0;
    
    LPWSTR* argv;
    int argc;
    argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    if (argv[1])
    {
        std::wstring path = argv[1];
        std::wstring privatekey = path + L"\\private.key";
        std::wstring publickey = path + L"\\public.key";

        std::string _privatekey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(privatekey);
        std::string _publickey = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(publickey);
        key_gen(&hKey, 2048);

        ULONG privKeySize;
        ULONG pubKeySize;
        export_privkey(hKey, privateKey, privKeySize, privatekey);
        export_pubkey(hKey, publicKey, pubKeySize, publickey);
    }
    else {
        MessageBoxA(0, "Path not specified", "Critical Error", 0);
    }
    return 0;
}