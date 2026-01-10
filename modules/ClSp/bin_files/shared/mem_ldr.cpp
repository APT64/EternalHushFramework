#define _CRT_SECURE_NO_WARNINGS
#define DEBUG_OUTPUT
#include <windows.h>
#include <winnt.h>
#include <stddef.h>
#include <tchar.h>
#ifdef DEBUG_OUTPUT
#include <stdio.h>
#endif
#include "mem_ldr.h"
#include "intstructs.h"
#include "cutils.h"
#include <syslib.hpp>
#include <Psapi.h>
extern syslib* psyslib;

static inline uintptr_t
AlignValueDown(uintptr_t value, uintptr_t alignment) {
    return value & ~(alignment - 1);
}

static inline LPVOID
AlignAddressDown(LPVOID address, uintptr_t alignment) {
    return (LPVOID)AlignValueDown((uintptr_t)address, alignment);
}

static inline size_t
AlignValueUp(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

static inline void*
OffsetPointer(void* data, ptrdiff_t offset) {
    return (void*)((uintptr_t)data + offset);
}


#ifdef _WIN64
static void
FreePointerList(POINTER_LIST* head, CustomFreeFunc freeMemory, void* userdata)
{
    POINTER_LIST* node = head;
    while (node) {
        POINTER_LIST* next;
        freeMemory(node->address, 0, MEM_RELEASE, userdata);
        next = node->next;
        free(node);
        node = next;
    }
}
#endif

static BOOL
CheckSize(size_t size, size_t expected) {
    if (size < expected) {
        SetLastError(ERROR_INVALID_DATA);
        return FALSE;
    }

    return TRUE;
}

static BOOL
CopySections(const unsigned char* data, size_t size, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{
    int i, section_size;
    unsigned char* codeBase = module->codeBase;
    unsigned char* dest;
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
    for (i = 0; i < module->headers->FileHeader.NumberOfSections; i++, section++) {
        if (section->SizeOfRawData == 0) {
            // section doesn't contain data in the dll itself, but may define
            // uninitialized data
            section_size = old_headers->OptionalHeader.SectionAlignment;
            if (section_size > 0) {
                dest = (unsigned char*)module->alloc(codeBase + section->VirtualAddress,
                    section_size,
                    MEM_COMMIT,
                    PAGE_READWRITE,
                    module->userdata);
                if (dest == NULL) {
                    return FALSE;
                }

                // Always use position from file to support alignments smaller
                // than page size (allocation above will align to page size).
                dest = codeBase + section->VirtualAddress;
                // NOTE: On 64bit systems we truncate to 32bit here but expand
                // again later when "PhysicalAddress" is used.
                section->Misc.PhysicalAddress = (DWORD)((uintptr_t)dest & 0xffffffff);
                _memset(dest, 0, section_size);
            }

            // section is empty
            continue;
        }

        if (!CheckSize(size, section->PointerToRawData + section->SizeOfRawData)) {
            return FALSE;
        }

        // commit memory block and copy data from dll
        dest = (unsigned char*)module->alloc(codeBase + section->VirtualAddress,
            section->SizeOfRawData,
            MEM_COMMIT,
            PAGE_READWRITE,
            module->userdata);
        if (dest == NULL) {
            return FALSE;
        }

        // Always use position from file to support alignments smaller
        // than page size (allocation above will align to page size).
        dest = codeBase + section->VirtualAddress;
        memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);
        // NOTE: On 64bit systems we truncate to 32bit here but expand
        // again later when "PhysicalAddress" is used.
        section->Misc.PhysicalAddress = (DWORD)((uintptr_t)dest & 0xffffffff);
    }

    return TRUE;
}

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] = {
    {
        // not executable
        {PAGE_NOACCESS, PAGE_WRITECOPY},
        {PAGE_READONLY, PAGE_READWRITE},
    }, {
        // executable
        {PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY},
        {PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE},
    },
};

static SIZE_T
GetRealSectionSize(PMEMORYMODULE module, PIMAGE_SECTION_HEADER section) {
    DWORD size = section->SizeOfRawData;
    if (size == 0) {
        if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
            size = module->headers->OptionalHeader.SizeOfInitializedData;
        }
        else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
            size = module->headers->OptionalHeader.SizeOfUninitializedData;
        }
    }
    return (SIZE_T)size;
}

static BOOL
FinalizeSection(PMEMORYMODULE module, PSECTIONFINALIZEDATA sectionData) {
    DWORD protect, oldProtect;
    BOOL executable;
    BOOL readable;
    BOOL writeable;

    if (sectionData->size == 0) {
        return TRUE;
    }

    if (sectionData->characteristics & IMAGE_SCN_MEM_DISCARDABLE) {
        // section is not needed any more and can safely be freed
        if (sectionData->address == sectionData->alignedAddress &&
            (sectionData->last ||
                module->headers->OptionalHeader.SectionAlignment == module->pageSize ||
                (sectionData->size % module->pageSize) == 0)
            ) {
            // Only allowed to decommit whole pages
            module->free(sectionData->address, sectionData->size, MEM_DECOMMIT, module->userdata);
        }
        return TRUE;
    }

    // determine protection flags based on characteristics
    executable = (sectionData->characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
    readable = (sectionData->characteristics & IMAGE_SCN_MEM_READ) != 0;
    writeable = (sectionData->characteristics & IMAGE_SCN_MEM_WRITE) != 0;
    protect = ProtectionFlags[executable][readable][writeable];
    if (sectionData->characteristics & IMAGE_SCN_MEM_NOT_CACHED) {
        protect |= PAGE_NOCACHE;
    }

    // change memory access flags
    if (VirtualProtect(sectionData->address, sectionData->size, protect, &oldProtect) == 0) {
        return FALSE;
    }

    return TRUE;
}

static BOOL
FinalizeSections(PMEMORYMODULE module)
{
    int i;
    PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
#ifdef _WIN64
    // "PhysicalAddress" might have been truncated to 32bit above, expand to
    // 64bits again.
    uintptr_t imageOffset = ((uintptr_t)module->headers->OptionalHeader.ImageBase & 0xffffffff00000000);
#else
    static const uintptr_t imageOffset = 0;
#endif
    SECTIONFINALIZEDATA sectionData;
    sectionData.address = (LPVOID)((uintptr_t)section->Misc.PhysicalAddress | imageOffset);
    sectionData.alignedAddress = AlignAddressDown(sectionData.address, module->pageSize);
    sectionData.size = GetRealSectionSize(module, section);
    sectionData.characteristics = section->Characteristics;
    sectionData.last = FALSE;
    section++;

    // loop through all sections and change access flags
    for (i = 1; i < module->headers->FileHeader.NumberOfSections; i++, section++) {
        LPVOID sectionAddress = (LPVOID)((uintptr_t)section->Misc.PhysicalAddress | imageOffset);
        LPVOID alignedAddress = AlignAddressDown(sectionAddress, module->pageSize);
        SIZE_T sectionSize = GetRealSectionSize(module, section);
        // Combine access flags of all sections that share a page
        // TODO(fancycode): We currently share flags of a trailing large section
        //   with the page of a first small section. This should be optimized.
        if (sectionData.alignedAddress == alignedAddress || (uintptr_t)sectionData.address + sectionData.size > (uintptr_t) alignedAddress) {
            // Section shares page with previous
            if ((section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0 || (sectionData.characteristics & IMAGE_SCN_MEM_DISCARDABLE) == 0) {
                sectionData.characteristics = (sectionData.characteristics | section->Characteristics) & ~IMAGE_SCN_MEM_DISCARDABLE;
            }
            else {
                sectionData.characteristics |= section->Characteristics;
            }
            sectionData.size = (((uintptr_t)sectionAddress) + ((uintptr_t)sectionSize)) - (uintptr_t)sectionData.address;
            continue;
        }

        if (!FinalizeSection(module, &sectionData)) {
            return FALSE;
        }
        sectionData.address = sectionAddress;
        sectionData.alignedAddress = alignedAddress;
        sectionData.size = sectionSize;
        sectionData.characteristics = section->Characteristics;
    }
    sectionData.last = TRUE;
    if (!FinalizeSection(module, &sectionData)) {
        return FALSE;
    }
    return TRUE;
}

static BOOL
ExecuteTLS(PMEMORYMODULE module)
{
    unsigned char* codeBase = module->codeBase;
    PIMAGE_TLS_DIRECTORY tls;
    PIMAGE_TLS_CALLBACK* callback;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_TLS);
    if (directory->VirtualAddress == 0) {
        return TRUE;
    }

    tls = (PIMAGE_TLS_DIRECTORY)(codeBase + directory->VirtualAddress);
    callback = (PIMAGE_TLS_CALLBACK*)tls->AddressOfCallBacks;
    if (callback) {
        while (*callback) {
            (*callback)((LPVOID)codeBase, DLL_PROCESS_ATTACH, NULL);
            callback++;
        }
    }
    return TRUE;
}

static BOOL
PerformBaseRelocation(PMEMORYMODULE module, ptrdiff_t delta)
{
    unsigned char* codeBase = module->codeBase;
    PIMAGE_BASE_RELOCATION relocation;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
    if (directory->Size == 0) {
        return (delta == 0);
    }

    relocation = (PIMAGE_BASE_RELOCATION)(codeBase + directory->VirtualAddress);
    for (; relocation->VirtualAddress > 0; ) {
        DWORD i;
        unsigned char* dest = codeBase + relocation->VirtualAddress;
        unsigned short* relInfo = (unsigned short*)OffsetPointer(relocation, IMAGE_SIZEOF_BASE_RELOCATION);
        for (i = 0; i < ((relocation->SizeOfBlock - IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++) {
            // the upper 4 bits define the type of relocation
            int type = *relInfo >> 12;
            // the lower 12 bits define the offset
            int offset = *relInfo & 0xfff;

            switch (type)
            {
            case IMAGE_REL_BASED_ABSOLUTE:
                // skip relocation
                break;

            case IMAGE_REL_BASED_HIGHLOW:
                // change complete 32 bit address
            {
                DWORD* patchAddrHL = (DWORD*)(dest + offset);
                *patchAddrHL += (DWORD)delta;
            }
            break;

#ifdef _WIN64
            case IMAGE_REL_BASED_DIR64:
            {
                ULONGLONG* patchAddr64 = (ULONGLONG*)(dest + offset);
                *patchAddr64 += (ULONGLONG)delta;
            }
            break;
#endif

            default:
                //printf("Unknown relocation: %d\n", type);
                break;
            }
        }

        // advance to next relocation block
        relocation = (PIMAGE_BASE_RELOCATION)OffsetPointer(relocation, relocation->SizeOfBlock);
    }
    return TRUE;
}

static BOOL
BuildImportTable(PMEMORYMODULE module)
{
    unsigned char* codeBase = module->codeBase;
    PIMAGE_IMPORT_DESCRIPTOR importDesc;
    BOOL result = TRUE;

    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
    if (directory->Size == 0) {
        return TRUE;
    }

    importDesc = (PIMAGE_IMPORT_DESCRIPTOR)(codeBase + directory->VirtualAddress);
    for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++) {
        uintptr_t* thunkRef;
        FARPROC* funcRef;
        HCUSTOMMODULE* tmp;
        HCUSTOMMODULE handle = module->loadLibrary((LPCSTR)(codeBase + importDesc->Name), module->userdata);
        if (handle == NULL) {
            SetLastError(ERROR_MOD_NOT_FOUND);
            result = FALSE;
            break;
        }

        tmp = (HCUSTOMMODULE*)realloc(module->modules, (module->numModules + 1) * (sizeof(HCUSTOMMODULE)));
        if (tmp == NULL) {
            module->freeLibrary(handle, module->userdata);
            SetLastError(ERROR_OUTOFMEMORY);
            result = FALSE;
            break;
        }
        module->modules = tmp;

        module->modules[module->numModules++] = handle;
        if (importDesc->OriginalFirstThunk) {
            thunkRef = (uintptr_t*)(codeBase + importDesc->OriginalFirstThunk);
            funcRef = (FARPROC*)(codeBase + importDesc->FirstThunk);
        }
        else {
            // no hint table
            thunkRef = (uintptr_t*)(codeBase + importDesc->FirstThunk);
            funcRef = (FARPROC*)(codeBase + importDesc->FirstThunk);
        }
        for (; *thunkRef; thunkRef++, funcRef++) {
            if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
                *funcRef = module->getProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef), module->userdata);
            }
            else {
                PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)(codeBase + (*thunkRef));
                *funcRef = module->getProcAddress(handle, (LPCSTR)&thunkData->Name, module->userdata);
            }
            if (*funcRef == 0) {
                result = FALSE;
                break;
            }
        }

        if (!result) {
            module->freeLibrary(handle, module->userdata);
            SetLastError(ERROR_PROC_NOT_FOUND);
            break;
        }
    }

    return result;
}

LPVOID MemoryDefaultAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata)
{
    UNREFERENCED_PARAMETER(userdata);
    LPVOID ptr = address;
    SIZE_T memsize = size;
    NTSTATUS status = psyslib->nt_call(HASH_NtAllocateVirtualMemory, (HANDLE)(-1), &ptr, NULL, &memsize, allocationType, protect);
    if (!NT_SUCCESS(status)) ptr = NULL;
    return ptr;
}

BOOL MemoryDefaultFree(LPVOID lpAddress, SIZE_T dwSize, DWORD dwFreeType, void* userdata)
{
    UNREFERENCED_PARAMETER(userdata);
    LPVOID ptr = lpAddress;
    SIZE_T memsize = dwSize;
    NTSTATUS status = psyslib->nt_call(HASH_NtFreeVirtualMemory, (HANDLE)(-1), &ptr, &memsize, dwFreeType);
    if (NT_SUCCESS(status)) return TRUE;
    return FALSE;
}

HCUSTOMMODULE MemoryDefaultLoadLibrary(LPCSTR filename, void* userdata)
{
    HMODULE result;
    UNREFERENCED_PARAMETER(userdata);
    result = LoadLibraryA(filename);
    if (result == NULL) {
        return NULL;
    }

    return (HCUSTOMMODULE)result;
}

FARPROC MemoryDefaultGetProcAddress(HCUSTOMMODULE module, LPCSTR name, void* userdata)
{
    UNREFERENCED_PARAMETER(userdata);
    FARPROC addr = 0;
    unsigned char* codeBase = (PUCHAR)module;
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)codeBase;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)codeBase + dosHeader->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)codeBase +
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    DWORD* addressOfFunctions = (DWORD*)((BYTE*)codeBase + exportDirectory->AddressOfFunctions);
    WORD* addressOfNameOrdinals = (WORD*)((BYTE*)codeBase + exportDirectory->AddressOfNameOrdinals);
    DWORD* addressOfNames = (DWORD*)((BYTE*)codeBase + exportDirectory->AddressOfNames);

    if (HIWORD(name) == 0) {
        DWORD idx = LOWORD(name) - exportDirectory->Base;
        addr = (FARPROC)(LPVOID)(codeBase + (*(DWORD*)(codeBase + exportDirectory->AddressOfFunctions + (idx * 4))));
    }
    else {
        for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
            PCHAR fname = (char*)codeBase + addressOfNames[i];
            if (_strcmp(name, fname) == 0) {
                addr = (FARPROC)((BYTE*)codeBase + addressOfFunctions[addressOfNameOrdinals[i]]);
            }
        }
    }
    if ((SIZE_T*)addr >= (SIZE_T*)exportDirectory && (SIZE_T*)addr < (SIZE_T*)exportDirectory + ntHeaders->OptionalHeader.DataDirectory->Size)
    {
        char* c;
        char *dllName = new char[256];
        HMODULE hForward = 0;
        c = (char*)addr;
        if (!c) return NULL;
        addr = NULL;
        int i = 0;
        while (c[i] != '.'){
            dllName[i] = c[i];
            i++;
        }
        c += i + 1;
        dllName[i++] = '.';
        dllName[i++] = 'd';
        dllName[i++] = 'l';
        dllName[i++] = 'l';
        dllName[i] = 0;
        UINT_PTR num = 0;
        if (*c == '#'){
            while (*++c) num = num * 10 + *c - '0';
        }
        else {
            num = (UINT_PTR)c;
        }
        hForward = LoadLibraryA(dllName);
        if (!hForward) return NULL;
        addr = GetProcAddress(hForward, (LPCSTR)num);
    }
    return addr;
}

void MemoryDefaultFreeLibrary(HCUSTOMMODULE module, void* userdata)
{
    UNREFERENCED_PARAMETER(userdata);
    FreeLibrary((HMODULE)module);
}

PMEMORYMODULE MemoryLoadLibrary(const void* data, size_t size)
{
    return MemoryLoadLibraryEx(data, size, MemoryDefaultAlloc, MemoryDefaultFree, MemoryDefaultLoadLibrary, MemoryDefaultGetProcAddress, MemoryDefaultFreeLibrary, NULL);
}

PMEMORYMODULE MemoryLoadLibraryEx(const void* data, size_t size,
    CustomAllocFunc allocMemory,
    CustomFreeFunc freeMemory,
    CustomLoadLibraryFunc loadLibrary,
    CustomGetProcAddressFunc getProcAddress,
    CustomFreeLibraryFunc freeLibrary,
    void* userdata)
{
    PMEMORYMODULE result = NULL;
    PIMAGE_DOS_HEADER dos_header;
    PIMAGE_NT_HEADERS old_header;
    unsigned char* code, * headers;
    ptrdiff_t locationDelta;
    SYSTEM_INFO sysInfo;
    PIMAGE_SECTION_HEADER section;
    DWORD i;
    size_t optionalSectionSize;
    size_t lastSectionEnd = 0;
    size_t alignedImageSize;
#ifdef _WIN64
    POINTER_LIST* blockedMemory = NULL;
#endif

    if (!CheckSize(size, sizeof(IMAGE_DOS_HEADER))) {
        return NULL;
    }
    dos_header = (PIMAGE_DOS_HEADER)data;
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    if (!CheckSize(size, dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS))) {
        return NULL;
    }
    old_header = (PIMAGE_NT_HEADERS) & ((const unsigned char*)(data))[dos_header->e_lfanew];
    if (old_header->Signature != IMAGE_NT_SIGNATURE) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    if (old_header->FileHeader.Machine != HOST_MACHINE) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    if (old_header->OptionalHeader.SectionAlignment & 1) {
        // Only support section alignments that are a multiple of 2
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    section = IMAGE_FIRST_SECTION(old_header);
    optionalSectionSize = old_header->OptionalHeader.SectionAlignment;
    for (i = 0; i < old_header->FileHeader.NumberOfSections; i++, section++) {
        size_t endOfSection;
        if (section->SizeOfRawData == 0) {
            // Section without data in the DLL
            endOfSection = section->VirtualAddress + optionalSectionSize;
        }
        else {
            endOfSection = section->VirtualAddress + section->SizeOfRawData;
        }

        if (endOfSection > lastSectionEnd) {
            lastSectionEnd = endOfSection;
        }
    }

    GetNativeSystemInfo(&sysInfo);
    alignedImageSize = AlignValueUp(old_header->OptionalHeader.SizeOfImage, sysInfo.dwPageSize);
    if (alignedImageSize != AlignValueUp(lastSectionEnd, sysInfo.dwPageSize)) {
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }

    // reserve memory for image of library
    // XXX: is it correct to commit the complete memory region at once?
    //      calling DllEntry raises an exception if we don't...
    code = (unsigned char*)allocMemory((LPVOID)(old_header->OptionalHeader.ImageBase),
        alignedImageSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE,
        userdata);

    if (code == NULL) {
        // try to allocate memory at arbitrary position
        code = (unsigned char*)allocMemory(NULL,
            alignedImageSize,
            MEM_RESERVE | MEM_COMMIT,
            PAGE_READWRITE,
            userdata);
        if (code == NULL) {
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }
    }

#ifdef _WIN64
    // Memory block may not span 4 GB boundaries.
    while ((((uintptr_t)code) >> 32) < (((uintptr_t)(code + alignedImageSize)) >> 32)) {
        POINTER_LIST* node = (POINTER_LIST*)malloc(sizeof(POINTER_LIST));
        if (!node) {
            freeMemory(code, 0, MEM_RELEASE, userdata);
            FreePointerList(blockedMemory, freeMemory, userdata);
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }

        node->next = blockedMemory;
        node->address = code;
        blockedMemory = node;

        code = (unsigned char*)allocMemory(NULL,
            alignedImageSize,
            MEM_RESERVE | MEM_COMMIT,
            PAGE_READWRITE,
            userdata);
        if (code == NULL) {
            FreePointerList(blockedMemory, freeMemory, userdata);
            SetLastError(ERROR_OUTOFMEMORY);
            return NULL;
        }
    }
#endif

    result = (PMEMORYMODULE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MEMORYMODULE));
    if (result == NULL) {
        freeMemory(code, 0, MEM_RELEASE, userdata);
#ifdef _WIN64
        FreePointerList(blockedMemory, freeMemory, userdata);
#endif
        SetLastError(ERROR_OUTOFMEMORY);
        return NULL;
    }

    result->codeBase = code;
    result->isDLL = (old_header->FileHeader.Characteristics & IMAGE_FILE_DLL) != 0;
    result->alloc = allocMemory;
    result->free = freeMemory;
    result->loadLibrary = loadLibrary;
    result->getProcAddress = getProcAddress;
    result->freeLibrary = freeLibrary;
    result->userdata = userdata;
    result->pageSize = sysInfo.dwPageSize;
#ifdef _WIN64
    result->blockedMemory = blockedMemory;
#endif

    if (!CheckSize(size, old_header->OptionalHeader.SizeOfHeaders)) {
        goto error;
    }

    // commit memory for headers
    headers = (unsigned char*)allocMemory(code,
        old_header->OptionalHeader.SizeOfHeaders,
        MEM_COMMIT,
        PAGE_READWRITE,
        userdata);

    // copy PE header to code
    memcpy(headers, dos_header, old_header->OptionalHeader.SizeOfHeaders);
    result->headers = (PIMAGE_NT_HEADERS) & ((const unsigned char*)(headers))[dos_header->e_lfanew];

    // update position
    result->headers->OptionalHeader.ImageBase = (uintptr_t)code;

    // copy sections from DLL file block to new memory location
    if (!CopySections((const unsigned char*)data, size, old_header, result)) {
        goto error;
    }

    // adjust base address of imported data
    locationDelta = (ptrdiff_t)(result->headers->OptionalHeader.ImageBase - old_header->OptionalHeader.ImageBase);
    if (locationDelta != 0) {
        result->isRelocated = PerformBaseRelocation(result, locationDelta);
    }
    else {
        result->isRelocated = TRUE;
    }

    // load required dlls and adjust function table of imports
    if (!BuildImportTable(result)) {
        goto error;
    }

    // mark memory pages depending on section headers and release
    // sections that are marked as "discardable"
    if (!FinalizeSections(result)) {
        goto error;
    }

    // TLS callbacks are executed BEFORE the main loading
    if (!ExecuteTLS(result)) {
        goto error;
    }

    // get entry point of loaded library
    if (result->headers->OptionalHeader.AddressOfEntryPoint != 0) {
        if (result->isDLL) {
            DllEntryProc DllEntry = (DllEntryProc)(LPVOID)(code + result->headers->OptionalHeader.AddressOfEntryPoint);
            // notify library about attaching to process
            BOOL successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);
            if (!successfull) {
                SetLastError(ERROR_DLL_INIT_FAILED);
                goto error;
            }
            result->initialized = TRUE;
        }
        else {
            result->exeEntry = (ExeEntryProc)(LPVOID)(code + result->headers->OptionalHeader.AddressOfEntryPoint);
        }
    }
    else {
        result->exeEntry = NULL;
    }

    return (PMEMORYMODULE)result;

error:
    // cleanup
    MemoryFreeLibrary(result);
    return NULL;
}

static int _compare(const void* a, const void* b)
{
    const struct ExportNameEntry* p1 = (const struct ExportNameEntry*)a;
    const struct ExportNameEntry* p2 = (const struct ExportNameEntry*)b;
    return _strcmp(p1->name, p2->name);
}

static int _find(const void* a, const void* b)
{
    LPCSTR* name = (LPCSTR*)a;
    const struct ExportNameEntry* p = (const struct ExportNameEntry*)b;
    return _strcmp(*name, p->name);
}
FARPROC MemoryGetProcAddress(PMEMORYMODULE mod, DWORD32 hash) {
    PMEMORYMODULE module = (PMEMORYMODULE)mod;
    return MemoryGetProcAddressP(module->codeBase, hash);
}
FARPROC MemoryGetProcAddressP(unsigned char* base, DWORD32 hash)
{
    FARPROC addr = 0;
    unsigned char* codeBase = (PUCHAR)base;
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)codeBase;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)codeBase + dosHeader->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)codeBase +
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    DWORD* addressOfFunctions = (DWORD*)((BYTE*)codeBase + exportDirectory->AddressOfFunctions);
    WORD* addressOfNameOrdinals = (WORD*)((BYTE*)codeBase + exportDirectory->AddressOfNameOrdinals);
    DWORD* addressOfNames = (DWORD*)((BYTE*)codeBase + exportDirectory->AddressOfNames);

    if (HIWORD(hash) == 0) {
        DWORD idx = LOWORD(hash) - exportDirectory->Base;
        addr = (FARPROC)(LPVOID)(codeBase + (*(DWORD*)(codeBase + exportDirectory->AddressOfFunctions + (idx * 4))));
    }
    else {
        for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
            PCHAR fname = (char*)codeBase + addressOfNames[i];
            if (default_hashfunc(fname, _strlen(fname)) == hash) {
                addr = (FARPROC)((BYTE*)codeBase + addressOfFunctions[addressOfNameOrdinals[i]]);
                break;
            }
        }
    }
    return addr;
}

void MemoryFreeLibrary(PMEMORYMODULE mod)
{
    PMEMORYMODULE module = (PMEMORYMODULE)mod;

    if (module == NULL) {
        return;
    }
    if (module->initialized) {
        // notify library about detaching from process
        DllEntryProc DllEntry = (DllEntryProc)(LPVOID)(module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
        (*DllEntry)((HINSTANCE)module->codeBase, DLL_PROCESS_DETACH, 0);
    }

    free(module->nameExportsTable);
    if (module->modules != NULL) {
        // free previously opened libraries
        int i;
        for (i = 0; i < module->numModules; i++) {
            if (module->modules[i] != NULL) {
                module->freeLibrary(module->modules[i], module->userdata);
            }
        }

        free(module->modules);
    }

    if (module->codeBase != NULL) {
        // release memory of library
        module->free(module->codeBase, 0, MEM_RELEASE, module->userdata);
    }

#ifdef _WIN64
    FreePointerList(module->blockedMemory, module->free, module->userdata);
#endif
    HeapFree(GetProcessHeap(), 0, module);
}

int MemoryCallEntryPoint(PMEMORYMODULE mod)
{
    PMEMORYMODULE module = (PMEMORYMODULE)mod;

    if (module == NULL || module->isDLL || module->exeEntry == NULL || !module->isRelocated) {
        return -1;
    }

    return module->exeEntry();
}

#define DEFAULT_LANGUAGE        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL)

HMEMORYRSRC MemoryFindResource(PMEMORYMODULE module, LPCTSTR name, LPCTSTR type)
{
    return MemoryFindResourceEx(module, name, type, DEFAULT_LANGUAGE);
}

static PIMAGE_RESOURCE_DIRECTORY_ENTRY _MemorySearchResourceEntry(
    void* root,
    PIMAGE_RESOURCE_DIRECTORY resources,
    LPCTSTR key)
{
    PIMAGE_RESOURCE_DIRECTORY_ENTRY entries = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(resources + 1);
    PIMAGE_RESOURCE_DIRECTORY_ENTRY result = NULL;
    DWORD start;
    DWORD end;
    DWORD middle;

    if (!IS_INTRESOURCE(key) && key[0] == TEXT('#')) {
        // special case: resource id given as string
        TCHAR* endpos = NULL;
        long int tmpkey = (WORD)_tcstol((TCHAR*)&key[1], &endpos, 10);
        if (tmpkey <= 0xffff && lstrlen(endpos) == 0) {
            key = MAKEINTRESOURCE(tmpkey);
        }
    }

    // entries are stored as ordered list of named entries,
    // followed by an ordered list of id entries - we can do
    // a binary search to find faster...
    if (IS_INTRESOURCE(key)) {
        WORD check = (WORD)(uintptr_t)key;
        start = resources->NumberOfNamedEntries;
        end = start + resources->NumberOfIdEntries;

        while (end > start) {
            WORD entryName;
            middle = (start + end) >> 1;
            entryName = (WORD)entries[middle].Name;
            if (check < entryName) {
                end = (end != middle ? middle : middle - 1);
            }
            else if (check > entryName) {
                start = (start != middle ? middle : middle + 1);
            }
            else {
                result = &entries[middle];
                break;
            }
        }
    }
    else {
        LPCWSTR searchKey;
        size_t searchKeyLen = _tcslen(key);
#if defined(UNICODE)
        searchKey = key;
#else
        // Resource names are always stored using 16bit characters, need to
        // convert string we search for.
#define MAX_LOCAL_KEY_LENGTH 2048
        // In most cases resource names are short, so optimize for that by
        // using a pre-allocated array.
        wchar_t _searchKeySpace[MAX_LOCAL_KEY_LENGTH + 1];
        LPWSTR _searchKey;
        if (searchKeyLen > MAX_LOCAL_KEY_LENGTH) {
            size_t _searchKeySize = (searchKeyLen + 1) * sizeof(wchar_t);
            _searchKey = (LPWSTR)malloc(_searchKeySize);
            if (_searchKey == NULL) {
                SetLastError(ERROR_OUTOFMEMORY);
                return NULL;
            }
        }
        else {
            _searchKey = &_searchKeySpace[0];
        }

        mbstowcs(_searchKey, key, searchKeyLen);
        _searchKey[searchKeyLen] = 0;
        searchKey = _searchKey;
#endif
        start = 0;
        end = resources->NumberOfNamedEntries;
        while (end > start) {
            int cmp;
            PIMAGE_RESOURCE_DIR_STRING_U resourceString;
            middle = (start + end) >> 1;
            resourceString = (PIMAGE_RESOURCE_DIR_STRING_U)OffsetPointer(root, entries[middle].Name & 0x7FFFFFFF);
            cmp = _wcsnicmp(searchKey, resourceString->NameString, resourceString->Length);
            if (cmp == 0) {
                // Handle partial match
                if (searchKeyLen > resourceString->Length) {
                    cmp = 1;
                }
                else if (searchKeyLen < resourceString->Length) {
                    cmp = -1;
                }
            }
            if (cmp < 0) {
                end = (middle != end ? middle : middle - 1);
            }
            else if (cmp > 0) {
                start = (middle != start ? middle : middle + 1);
            }
            else {
                result = &entries[middle];
                break;
            }
        }
#if !defined(UNICODE)
        if (searchKeyLen > MAX_LOCAL_KEY_LENGTH) {
            free(_searchKey);
        }
#undef MAX_LOCAL_KEY_LENGTH
#endif
    }

    return result;
}

HMEMORYRSRC MemoryFindResourceEx(PMEMORYMODULE module, LPCTSTR name, LPCTSTR type, WORD language)
{
    unsigned char* codeBase = ((PMEMORYMODULE)module)->codeBase;
    PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY((PMEMORYMODULE)module, IMAGE_DIRECTORY_ENTRY_RESOURCE);
    PIMAGE_RESOURCE_DIRECTORY rootResources;
    PIMAGE_RESOURCE_DIRECTORY nameResources;
    PIMAGE_RESOURCE_DIRECTORY typeResources;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY foundType;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY foundName;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY foundLanguage;
    if (directory->Size == 0) {
        // no resource table found
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return NULL;
    }

    if (language == DEFAULT_LANGUAGE) {
        // use language from current thread
        language = LANGIDFROMLCID(GetThreadLocale());
    }

    // resources are stored as three-level tree
    // - first node is the type
    // - second node is the name
    // - third node is the language
    rootResources = (PIMAGE_RESOURCE_DIRECTORY)(codeBase + directory->VirtualAddress);
    foundType = _MemorySearchResourceEntry(rootResources, rootResources, type);
    if (foundType == NULL) {
        SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
        return NULL;
    }

    typeResources = (PIMAGE_RESOURCE_DIRECTORY)(codeBase + directory->VirtualAddress + (foundType->OffsetToData & 0x7fffffff));
    foundName = _MemorySearchResourceEntry(rootResources, typeResources, name);
    if (foundName == NULL) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return NULL;
    }

    nameResources = (PIMAGE_RESOURCE_DIRECTORY)(codeBase + directory->VirtualAddress + (foundName->OffsetToData & 0x7fffffff));
    foundLanguage = _MemorySearchResourceEntry(rootResources, nameResources, (LPCTSTR)(uintptr_t)language);
    if (foundLanguage == NULL) {
        // requested language not found, use first available
        if (nameResources->NumberOfIdEntries == 0) {
            SetLastError(ERROR_RESOURCE_LANG_NOT_FOUND);
            return NULL;
        }

        foundLanguage = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(nameResources + 1);
    }

    return (codeBase + directory->VirtualAddress + (foundLanguage->OffsetToData & 0x7fffffff));
}

DWORD MemorySizeofResource(PMEMORYMODULE module, HMEMORYRSRC resource)
{
    PIMAGE_RESOURCE_DATA_ENTRY entry;
    UNREFERENCED_PARAMETER(module);
    entry = (PIMAGE_RESOURCE_DATA_ENTRY)resource;
    if (entry == NULL) {
        return 0;
    }

    return entry->Size;
}

LPVOID MemoryLoadResource(PMEMORYMODULE module, HMEMORYRSRC resource)
{
    unsigned char* codeBase = ((PMEMORYMODULE)module)->codeBase;
    PIMAGE_RESOURCE_DATA_ENTRY entry = (PIMAGE_RESOURCE_DATA_ENTRY)resource;
    if (entry == NULL) {
        return NULL;
    }

    return codeBase + entry->OffsetToData;
}

int
MemoryLoadString(PMEMORYMODULE module, UINT id, LPTSTR buffer, int maxsize)
{
    return MemoryLoadStringEx(module, id, buffer, maxsize, DEFAULT_LANGUAGE);
}

int
MemoryLoadStringEx(PMEMORYMODULE module, UINT id, LPTSTR buffer, int maxsize, WORD language)
{
    HMEMORYRSRC resource;
    PIMAGE_RESOURCE_DIR_STRING_U data;
    DWORD size;
    if (maxsize == 0) {
        return 0;
    }

    resource = MemoryFindResourceEx(module, MAKEINTRESOURCE((id >> 4) + 1), RT_STRING, language);
    if (resource == NULL) {
        buffer[0] = 0;
        return 0;
    }

    data = (PIMAGE_RESOURCE_DIR_STRING_U)MemoryLoadResource(module, resource);
    id = id & 0x0f;
    while (id--) {
        data = (PIMAGE_RESOURCE_DIR_STRING_U)OffsetPointer(data, (data->Length + 1) * sizeof(WCHAR));
    }
    if (data->Length == 0) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        buffer[0] = 0;
        return 0;
    }

    size = data->Length;
    if (size >= (DWORD)maxsize) {
        size = maxsize;
    }
    else {
        buffer[size] = 0;
    }
#if defined(UNICODE)
    wcsncpy(buffer, data->NameString, size);
#else
    wcstombs(buffer, data->NameString, size);
#endif
    return size;
}


HMODULE MemoryGetModuleHandle(DWORD32 hash) {
    PEB* pPeb = NtCurrentPeb();
    // obtaining the address of the head node in a linked list 
    // which represents all the models that are loaded into the process.
    PEB_LDR_DATA* Ldr = pPeb->Ldr;
    LIST_ENTRY* ModuleList = &Ldr->InLoadOrderModuleList;

    // iterating to the next node. this will be our starting point.
    LIST_ENTRY* pStartListEntry = ModuleList->Flink;

    for (LIST_ENTRY* pListEntry = pStartListEntry; pListEntry != ModuleList; pListEntry = pListEntry->Flink) {
        // getting the address of current LDR_DATA_TABLE_ENTRY (which represents the DLL).
        PLDR_DATA_TABLE_ENTRY pEntry = (PLDR_DATA_TABLE_ENTRY)pListEntry;
        DWORD namelen = _wcslen(pEntry->BaseDllName.Buffer) * 2;
        WCHAR* low_name = new wchar_t[namelen + 2];
        _strlow((char*)(pEntry->BaseDllName.Buffer), namelen + 2, (char*)low_name);
        if (default_hashfunc((char*)low_name, namelen) == hash) {
            delete[] low_name;
            return (HMODULE)pEntry->DllBase;
        }
        delete[] low_name;
    }
    return NULL;
}

DWORD64 MemoryGetModuleSize(DWORD32 hash) {
    PEB* pPeb = NtCurrentPeb();
    // obtaining the address of the head node in a linked list 
    // which represents all the models that are loaded into the process.
    PEB_LDR_DATA* Ldr = pPeb->Ldr;
    LIST_ENTRY* ModuleList = &Ldr->InLoadOrderModuleList;

    // iterating to the next node. this will be our starting point.
    LIST_ENTRY* pStartListEntry = ModuleList->Flink;

    for (LIST_ENTRY* pListEntry = pStartListEntry; pListEntry != ModuleList; pListEntry = pListEntry->Flink) {
        // getting the address of current LDR_DATA_TABLE_ENTRY (which represents the DLL).
        PLDR_DATA_TABLE_ENTRY pEntry = (PLDR_DATA_TABLE_ENTRY)pListEntry;
        DWORD namelen = _wcslen(pEntry->BaseDllName.Buffer) * 2;
        WCHAR* low_name = new wchar_t[namelen + 2];
        _strlow((char*)(pEntry->BaseDllName.Buffer), namelen + 2, (char*)low_name);
        if (default_hashfunc((char*)low_name, namelen) == hash) {
            delete[] low_name;
            return pEntry->SizeOfImage;
        }
        delete[] low_name;
    }
    return NULL;
}

PEB* NtCurrentPeb() {
#ifdef _WIN64
    PEB* pPeb = (PEB*)__readgsqword(0x60);
#else
    PEB* pPeb = (PEB*)__readfsdword(0x30);
#endif
    return pPeb;
}


HMODULE WINAPI GetRemoteModuleHandle(HANDLE hProcess, DWORD32 name_hash)
{
    HMODULE* ModuleArray = NULL;
    DWORD ModuleArraySize = 100;
    DWORD NumModules = 0;
    CHAR lpModuleNameCopy[MAX_PATH] = { 0 };
    CHAR ModuleNameBuffer[MAX_PATH] = { 0 };

    /* Allocate memory to hold the module handles */
    ModuleArray = new HMODULE[ModuleArraySize];

    /* Check if the allocation failed */
    if (ModuleArray == NULL)
        goto GRMH_FAIL_JMP;

    /* Get handles to all the modules in the target process */
    if (!EnumProcessModulesEx(hProcess, ModuleArray,
        ModuleArraySize * sizeof(HMODULE), &NumModules, LIST_MODULES_ALL))
        goto GRMH_FAIL_JMP;
    /* We want the number of modules not the number of bytes */
    NumModules /= sizeof(HMODULE);

    /* Did we allocate enough memory for all the module handles? */
    if (NumModules > ModuleArraySize)
    {
        delete[] ModuleArray; // Deallocate so we can try again
        ModuleArray = NULL; // Set it to NULL se we can be sure if the next try fails
        ModuleArray = new HMODULE[NumModules]; // Allocate the right amount of memory

        /* Check if the allocation failed */
        if (ModuleArray == NULL)
            goto GRMH_FAIL_JMP;

        ModuleArraySize = NumModules; // Update the size of the array

        /* Get handles to all the modules in the target process */
        if (!::EnumProcessModulesEx(hProcess, ModuleArray,
            ModuleArraySize * sizeof(HMODULE), &NumModules, LIST_MODULES_ALL))
            goto GRMH_FAIL_JMP;

        /* We want the number of modules not the number of bytes */
        NumModules /= sizeof(HMODULE);
    }

    /* Iterate through all the modules and see if the names match the one we are looking for */
    for (DWORD i = 0; i <= NumModules; ++i)
    {
        /* Get the module's name */
        GetModuleBaseNameA(hProcess, ModuleArray[i],
            ModuleNameBuffer, sizeof(ModuleNameBuffer));
        /* Convert ModuleNameBuffer to all lowercase so the comparison isn't case sensitive */
        for (size_t j = 0; ModuleNameBuffer[j] != '\0'; ++j)
        {
            if (ModuleNameBuffer[j] >= 'A' && ModuleNameBuffer[j] <= 'Z')
                ModuleNameBuffer[j] += 0x20; // 0x20 is the difference between uppercase and lowercase
        }
        /* Does the name match? */
        if (default_hashfunc((char*)ModuleNameBuffer, strlen(ModuleNameBuffer)) == name_hash)
        {
            /* Make a temporary variable to hold return value*/
            HMODULE TempReturn = ModuleArray[i];

            /* Give back that memory */
            delete[] ModuleArray;

            /* Success */
            return TempReturn;
        }

        /* Wrong module let's try the next... */
    }

    /* Uh Oh... */
GRMH_FAIL_JMP:

    /* If we got to the point where we allocated memory we need to give it back */
    if (ModuleArray != NULL)
        delete[] ModuleArray;

    /* Failure... */
    return NULL;
}


//-----------------------------------------------------------------------------

FARPROC WINAPI GetRemoteProcAddress(HANDLE hProcess, HMODULE hModule, DWORD32 name_hash, UINT Ordinal, BOOL UseOrdinal)
{
    BOOL Is64Bit = FALSE;
    MODULEINFO RemoteModuleInfo = { 0 };
    UINT_PTR RemoteModuleBaseVA = 0;
    IMAGE_DOS_HEADER DosHeader = { 0 };
    DWORD Signature = 0;
    IMAGE_FILE_HEADER FileHeader = { 0 };
    IMAGE_OPTIONAL_HEADER64 OptHeader64 = { 0 };
    IMAGE_OPTIONAL_HEADER32 OptHeader32 = { 0 };
    IMAGE_DATA_DIRECTORY ExportDirectory = { 0 };
    IMAGE_EXPORT_DIRECTORY ExportTable = { 0 };
    UINT_PTR ExportFunctionTableVA = 0;
    UINT_PTR ExportNameTableVA = 0;
    UINT_PTR ExportOrdinalTableVA = 0;
    DWORD* ExportFunctionTable = NULL;
    DWORD* ExportNameTable = NULL;
    WORD* ExportOrdinalTable = NULL;

    /* Temporary variables not used until much later but easier
    /* to define here than in all the the places they are used */
    CHAR TempChar;
    BOOL Done = FALSE;

    /* Get the base address of the remote module along with some other info we don't need */
    if (!::GetModuleInformation(hProcess, hModule, &RemoteModuleInfo, sizeof(RemoteModuleInfo)))
        goto GRPA_FAIL_JMP;
    RemoteModuleBaseVA = (UINT_PTR)RemoteModuleInfo.lpBaseOfDll;

    /* Read the DOS header and check it's magic number */
    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess, (LPCVOID)RemoteModuleBaseVA, &DosHeader,
        sizeof(DosHeader), NULL) || DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
        goto GRPA_FAIL_JMP;

    /* Read and check the NT signature */
    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess, (LPCVOID)(RemoteModuleBaseVA + DosHeader.e_lfanew),
        &Signature, sizeof(Signature), NULL) || Signature != IMAGE_NT_SIGNATURE)
        goto GRPA_FAIL_JMP;

    /* Read the main header */
    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess,
        (LPCVOID)(RemoteModuleBaseVA + DosHeader.e_lfanew + sizeof(Signature)),
        &FileHeader, sizeof(FileHeader), NULL))
        goto GRPA_FAIL_JMP;

    /* Which type of optional header is the right size? */
    if (FileHeader.SizeOfOptionalHeader == sizeof(OptHeader64))
        Is64Bit = TRUE;
    else if (FileHeader.SizeOfOptionalHeader == sizeof(OptHeader32))
        Is64Bit = FALSE;
    else
        goto GRPA_FAIL_JMP;

    if (Is64Bit)
    {
        /* Read the optional header and check it's magic number */
        if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess,
            (LPCVOID)(RemoteModuleBaseVA + DosHeader.e_lfanew + sizeof(Signature) + sizeof(FileHeader)),
            &OptHeader64, FileHeader.SizeOfOptionalHeader, NULL)
            || OptHeader64.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
            goto GRPA_FAIL_JMP;
    }
    else
    {
        /* Read the optional header and check it's magic number */
        if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess,
            (LPCVOID)(RemoteModuleBaseVA + DosHeader.e_lfanew + sizeof(Signature) + sizeof(FileHeader)),
            &OptHeader32, FileHeader.SizeOfOptionalHeader, NULL)
            || OptHeader32.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC)
            goto GRPA_FAIL_JMP;
    }

    /* Make sure the remote module has an export directory and if it does save it's relative address and size */
    if (Is64Bit && OptHeader64.NumberOfRvaAndSizes >= IMAGE_DIRECTORY_ENTRY_EXPORT + 1)
    {
        ExportDirectory.VirtualAddress = (OptHeader64.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).VirtualAddress;
        ExportDirectory.Size = (OptHeader64.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).Size;
    }
    else if (OptHeader32.NumberOfRvaAndSizes >= IMAGE_DIRECTORY_ENTRY_EXPORT + 1)
    {
        ExportDirectory.VirtualAddress = (OptHeader32.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).VirtualAddress;
        ExportDirectory.Size = (OptHeader32.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]).Size;
    }
    else
        goto GRPA_FAIL_JMP;

    /* Read the main export table */
    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess, (LPCVOID)(RemoteModuleBaseVA + ExportDirectory.VirtualAddress),
        &ExportTable, sizeof(ExportTable), NULL))
        goto GRPA_FAIL_JMP;

    /* Save the absolute address of the tables so we don't need to keep adding the base address */
    ExportFunctionTableVA = RemoteModuleBaseVA + ExportTable.AddressOfFunctions;
    ExportNameTableVA = RemoteModuleBaseVA + ExportTable.AddressOfNames;
    ExportOrdinalTableVA = RemoteModuleBaseVA + ExportTable.AddressOfNameOrdinals;

    /* Allocate memory for our copy of the tables */
    ExportFunctionTable = new DWORD[ExportTable.NumberOfFunctions];
    ExportNameTable = new DWORD[ExportTable.NumberOfNames];
    ExportOrdinalTable = new WORD[ExportTable.NumberOfNames];

    /* Check if the allocation failed */
    if (ExportFunctionTable == NULL || ExportNameTable == NULL || ExportOrdinalTable == NULL)
        goto GRPA_FAIL_JMP;

    /* Get a copy of the function table */
    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess, (LPCVOID)ExportFunctionTableVA,
        ExportFunctionTable, ExportTable.NumberOfFunctions * sizeof(DWORD), NULL))
        goto GRPA_FAIL_JMP;

    /* Get a copy of the name table */
    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess, (LPCVOID)ExportNameTableVA,
        ExportNameTable, ExportTable.NumberOfNames * sizeof(DWORD), NULL))
        goto GRPA_FAIL_JMP;

    /* Get a copy of the ordinal table */
    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess, (LPCVOID)ExportOrdinalTableVA,
        ExportOrdinalTable, ExportTable.NumberOfNames * sizeof(WORD), NULL))
        goto GRPA_FAIL_JMP;

    /* If we are searching for an ordinal we do that now */
    if (UseOrdinal)
    {
        /* NOTE:
        /* Microsoft's PE/COFF specification does NOT say we need to subtract the ordinal base
        /* from our ordinal but it seems to always give the wrong function if we don't */

        /* Make sure the ordinal is valid */
        if (Ordinal < ExportTable.Base || (Ordinal - ExportTable.Base) >= ExportTable.NumberOfFunctions)
            goto GRPA_FAIL_JMP;

        UINT FunctionTableIndex = Ordinal - ExportTable.Base;

        /* Check if the function is forwarded and if so get the real address*/
        if (ExportFunctionTable[FunctionTableIndex] >= ExportDirectory.VirtualAddress &&
            ExportFunctionTable[FunctionTableIndex] <= ExportDirectory.VirtualAddress + ExportDirectory.Size)
        {
            Done = FALSE;
            std::string TempForwardString;
            TempForwardString.clear(); // Empty the string so we can fill it with a new name

            /* Get the forwarder string one character at a time because we don't know how long it is */
            for (UINT_PTR i = 0; !Done; ++i)
            {
                /* Get next character */
                if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess,
                    (LPCVOID)(RemoteModuleBaseVA + ExportFunctionTable[FunctionTableIndex] + i),
                    &TempChar, sizeof(TempChar), NULL))
                    goto GRPA_FAIL_JMP;

                TempForwardString.push_back(TempChar); // Add it to the string

                /* If it's NUL we are done */
                if (TempChar == (CHAR)'\0')
                    Done = TRUE;
            }

            /* Find the dot that seperates the module name and the function name/ordinal */
            size_t Dot = TempForwardString.find('.');
            if (Dot == std::string::npos)
                goto GRPA_FAIL_JMP;

            /* Temporary variables that hold parts of the forwarder string */
            std::string RealModuleName, RealFunctionId;
            RealModuleName = TempForwardString.substr(0, Dot - 1);
            RealFunctionId = TempForwardString.substr(Dot + 1, std::string::npos);

            HMODULE RealModule = GetRemoteModuleHandle(hProcess, default_hashfunc((char*)RealModuleName.c_str(), RealFunctionId.size()));
            FARPROC TempReturn;// Make a temporary variable to hold return value 

            TempReturn = GetRemoteProcAddress(hProcess, RealModule, default_hashfunc((char*)RealFunctionId.c_str(), RealFunctionId.size()), 0, FALSE);
           
            /* Give back that memory */
            delete[] ExportFunctionTable;
            delete[] ExportNameTable;
            delete[] ExportOrdinalTable;

            /* Success!!! */
            return TempReturn;
        }
        else // Not Forwarded
        {

            /* Make a temporary variable to hold return value*/
            FARPROC TempReturn = (FARPROC)(RemoteModuleBaseVA + ExportFunctionTable[FunctionTableIndex]);

            /* Give back that memory */
            delete[] ExportFunctionTable;
            delete[] ExportNameTable;
            delete[] ExportOrdinalTable;

            /* Success!!! */
            return TempReturn;
        }
    }


    /* Iterate through all the names and see if they match the one we are looking for */
    for (DWORD i = 0; i < ExportTable.NumberOfNames; ++i) {
        std::string TempFunctionName;

        Done = FALSE;// Reset for next name
        TempFunctionName.clear(); // Empty the string so we can fill it with a new name

        /* Get the function name one character at a time because we don't know how long it is */
        for (UINT_PTR j = 0; !Done; ++j)
        {
            /* Get next character */
            if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess, (LPCVOID)(RemoteModuleBaseVA + ExportNameTable[i] + j),
                &TempChar, sizeof(TempChar), NULL))
                goto GRPA_FAIL_JMP;

            TempFunctionName.push_back(TempChar); // Add it to the string

            /* If it's NUL we are done */
            if (TempChar == (CHAR)'\0')
                Done = TRUE;
        }

        /* Does the name match? */
        if (default_hashfunc((char*)TempFunctionName.c_str(), TempFunctionName.size()-1) == name_hash)
        {
            /* NOTE:
            /* Microsoft's PE/COFF specification says we need to subtract the ordinal base
            /*from the value in the ordinal table but that seems to always give the wrong function */

            /* Check if the function is forwarded and if so get the real address*/
            if (ExportFunctionTable[ExportOrdinalTable[i]] >= ExportDirectory.VirtualAddress &&
                ExportFunctionTable[ExportOrdinalTable[i]] <= ExportDirectory.VirtualAddress + ExportDirectory.Size)
            {
                Done = FALSE;
                std::string TempForwardString;
                TempForwardString.clear(); // Empty the string so we can fill it with a new name

                /* Get the forwarder string one character at a time because we don't know how long it is */
                for (UINT_PTR j = 0; !Done; ++j)
                {
                    /* Get next character */
                    if (psyslib->nt_call(HASH_NtReadVirtualMemory, hProcess,
                        (LPCVOID)(RemoteModuleBaseVA + ExportFunctionTable[i] + j),
                        &TempChar, sizeof(TempChar), NULL))
                        goto GRPA_FAIL_JMP;

                    TempForwardString.push_back(TempChar); // Add it to the string

                    /* If it's NUL we are done */
                    if (TempChar == (CHAR)'\0')
                        Done = TRUE;
                }

                /* Find the dot that seperates the module name and the function name/ordinal */
                size_t Dot = TempForwardString.find('.');
                if (Dot == std::string::npos)
                    goto GRPA_FAIL_JMP;

                /* Temporary variables that hold parts of the forwarder string */
                std::string RealModuleName, RealFunctionId;
                RealModuleName = TempForwardString.substr(0, Dot);
                RealFunctionId = TempForwardString.substr(Dot + 1, std::string::npos);

                HMODULE RealModule = GetRemoteModuleHandle(hProcess, default_hashfunc((char*)RealModuleName.c_str(), RealFunctionId.size()));
                FARPROC TempReturn;// Make a temporary variable to hold return value 
                TempReturn = GetRemoteProcAddress(hProcess, RealModule, default_hashfunc((char*)RealFunctionId.c_str(), RealFunctionId.size()), 0, FALSE);
                
                /* Give back that memory */
                delete[] ExportFunctionTable;
                delete[] ExportNameTable;
                delete[] ExportOrdinalTable;

                /* Success!!! */
                return TempReturn;
            }
            else // Not Forwarded
            {

                /* Make a temporary variable to hold return value*/
                FARPROC TempReturn;

                /* NOTE:
                /* Microsoft's PE/COFF specification says we need to subtract the ordinal base
                /*from the value in the ordinal table but that seems to always give the wrong function */
                //TempReturn = (FARPROC)(RemoteModuleBaseVA + ExportFunctionTable[ExportOrdinalTable[i] - ExportTable.Base]);

                /* So we do it this way instead */
                TempReturn = (FARPROC)(RemoteModuleBaseVA + ExportFunctionTable[ExportOrdinalTable[i]]);

                /* Give back that memory */
                delete[] ExportFunctionTable;
                delete[] ExportNameTable;
                delete[] ExportOrdinalTable;

                /* Success!!! */
                return TempReturn;
            }
        }

        /* Wrong function let's try the next... */
    }

    /* Uh Oh... */
GRPA_FAIL_JMP:

    /* If we got to the point where we allocated memory we need to give it back */
    if (ExportFunctionTable != NULL)
        delete[] ExportFunctionTable;
    if (ExportNameTable != NULL)
        delete[] ExportNameTable;
    if (ExportOrdinalTable != NULL)
        delete[] ExportOrdinalTable;

    /* Falure... */
    return NULL;
}