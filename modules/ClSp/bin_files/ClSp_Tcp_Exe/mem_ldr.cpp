#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <mem_ldr.h>
#define NtHdr(base) ((IMAGE_NT_HEADERS *)((ULONGLONG)base + ((IMAGE_DOS_HEADER *)base)->e_lfanew))

int __strcmp(const char* str1, const char* str2) {
	while (*str1 || *str2) {
		if (*str1 < *str2) {
			return -1;
		}
		else if (*str1 > *str2) {
			return 1;
		}
		str1++;
		str2++;
	}
	return 0;
}


IMAGE_DATA_DIRECTORY* __reloc_table(PIMAGE_NT_HEADERS nt_hdr) {
	IMAGE_DATA_DIRECTORY* reloc_table = &nt_hdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	if (reloc_table->VirtualAddress == NULL) {
		return NULL;
	}
	else {
		return reloc_table;
	}
}



bool __iat_fix(PUCHAR base_addr) {
	IMAGE_NT_HEADERS* ntHeader = NtHdr(base_addr);
	IMAGE_DATA_DIRECTORY* iat_directory = &ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	if (iat_directory->VirtualAddress == NULL) {
		return false;
	}
	else {
		size_t iatSize = iat_directory->Size;
		size_t iatRVA = iat_directory->VirtualAddress;
		IMAGE_IMPORT_DESCRIPTOR* entry = NULL;
		size_t parsedSize = 0;
		for (; parsedSize < iatSize; parsedSize += sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
			entry = (IMAGE_IMPORT_DESCRIPTOR*)(iatRVA + (ULONG_PTR)base_addr + parsedSize);
			if (entry->OriginalFirstThunk == NULL && entry->FirstThunk == NULL) {
				break;
			}
			char* dll_name = (char*)((ULONGLONG)base_addr + entry->Name);
			size_t firstThunkRVA = entry->FirstThunk;
			size_t originalFirstThunkRVA = entry->OriginalFirstThunk;
			if (originalFirstThunkRVA == NULL) {
				originalFirstThunkRVA = entry->FirstThunk;
			}
			ULONG cursorFirstThunk = 0;
			while (true) {
				IMAGE_THUNK_DATA* firstThunkData = (IMAGE_THUNK_DATA*)(base_addr + cursorFirstThunk + firstThunkRVA);
				IMAGE_THUNK_DATA* originalFirstThunkData = (IMAGE_THUNK_DATA*)(base_addr + cursorFirstThunk + originalFirstThunkRVA);
				if (firstThunkData->u1.Function == NULL) {
					break;
				}
				else if (IMAGE_SNAP_BY_ORDINAL(originalFirstThunkData->u1.Ordinal)) {
					unsigned int printOrdinal = originalFirstThunkData->u1.Ordinal & 0xFFFF;
					PULONG functionAddr = (PULONG)GetProcAddress(LoadLibraryA(dll_name), (char*)(originalFirstThunkData->u1.Ordinal & 0xFFFF));
					firstThunkData->u1.Function = (ULONGLONG)functionAddr;
				}
				else {
					PIMAGE_IMPORT_BY_NAME func_name = (PIMAGE_IMPORT_BY_NAME)(size_t(base_addr) + originalFirstThunkData->u1.AddressOfData);
					PULONG dunc_addr = (PULONG)_GetProcAddress(LoadLibraryA(dll_name), func_name->Name);
					firstThunkData->u1.Function = (ULONGLONG)dunc_addr;
				}
				cursorFirstThunk += sizeof(IMAGE_THUNK_DATA);
			}
		}
	}
}


bool __reloc_fix(PUCHAR base_addr, BYTE* pref_addr, IMAGE_DATA_DIRECTORY* reloc) {
	PIMAGE_BASE_RELOCATION block_info = NULL;

	ULONGLONG relocBlockOffset = 0;
	for (; relocBlockOffset < reloc->Size; relocBlockOffset += block_info->SizeOfBlock) {
		block_info = (IMAGE_BASE_RELOCATION*)(reloc->VirtualAddress + relocBlockOffset + base_addr);
		if (block_info->VirtualAddress == NULL || block_info->SizeOfBlock == 0) {
			break;
		}
		ULONGLONG entriesNum = (block_info->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(BASE_RELOCATION_ENTRY);
		ULONGLONG pageStart = block_info->VirtualAddress;
		BASE_RELOCATION_ENTRY* reloc_entry = (BASE_RELOCATION_ENTRY*)((PUCHAR)block_info + sizeof(IMAGE_BASE_RELOCATION));
		for (int i = 0; i < entriesNum; i++) {
			if (reloc_entry->Type == 0) {
				continue;
			}
			PULONG reloc_addr = (PULONG)(pageStart + base_addr + reloc_entry->Offset);
			*reloc_addr = *reloc_addr + base_addr - pref_addr;
			reloc_entry = (BASE_RELOCATION_ENTRY*)((PUCHAR)reloc_entry + sizeof(BASE_RELOCATION_ENTRY));
		}
	}
	if (relocBlockOffset == 0) {
		return false;
	}
	return true;
}

PULONG _LoadLibrary(char* payload) {
	IMAGE_NT_HEADERS* nt_hdr = NtHdr(payload);
	IMAGE_DATA_DIRECTORY* reloc_table = __reloc_table(nt_hdr);
	ULONGLONG pref_base_addr = nt_hdr->OptionalHeader.ImageBase;
	HANDLE sectionHandle = NULL;
	PVOID sectionAddr = NULL;

	NTMAPVIEWOFSECTION pNtMapViewOfSection = (NTMAPVIEWOFSECTION)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtMapViewOfSection");
	NTCREATESECTION pNtCreateSection = (NTCREATESECTION)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtCreateSection");

	SIZE_T sectionMemSize = nt_hdr->OptionalHeader.SizeOfImage;
	LARGE_INTEGER sectionSize = { sectionMemSize };
	pNtCreateSection(&sectionHandle, SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE, NULL, &sectionSize, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL);
	pNtMapViewOfSection(sectionHandle, (HANDLE)-1, &sectionAddr, NULL, NULL, NULL, &sectionMemSize, ViewUnmap, NULL, PAGE_EXECUTE_READWRITE);
	DWORD old;
	VirtualProtect(sectionAddr, sectionMemSize, PAGE_READWRITE, &old);

	PUCHAR image_base = (PUCHAR)sectionAddr;
	if (!image_base && !reloc_table) {
		return NULL;
	}
	nt_hdr->OptionalHeader.ImageBase = (ULONGLONG)image_base;
	memcpy(sectionAddr, payload, nt_hdr->OptionalHeader.SizeOfHeaders);
	PIMAGE_SECTION_HEADER section_hdr = (PIMAGE_SECTION_HEADER)((long long)nt_hdr + sizeof(IMAGE_NT_HEADERS));

	for (int i = 0; i < nt_hdr->FileHeader.NumberOfSections; i++) {
		memcpy(image_base + section_hdr[i].VirtualAddress, payload + section_hdr[i].PointerToRawData, section_hdr[i].SizeOfRawData);
	}
	if (__iat_fix(image_base))
	{
		if (reloc_table) {
			if (__reloc_fix(image_base, (BYTE*)pref_base_addr, reloc_table)) {
				VirtualProtect(sectionAddr, sectionMemSize, PAGE_EXECUTE_READWRITE, &old);
				return (PULONG)sectionAddr;
			}
		}
	}
	return NULL;
}

void _UnloadLibrary(PVOID baseaddr) {
	NTUNMAPVIEWOFSECTION pNtUnmapViewOfSection = (NTUNMAPVIEWOFSECTION)GetProcAddress(LoadLibraryA("ntdll.dll"), "NtUnmapViewOfSection");
	pNtUnmapViewOfSection((HANDLE)-1, baseaddr);
}

FARPROC _GetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule +
		ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	DWORD* addressOfFunctions = (DWORD*)((BYTE*)hModule + exportDirectory->AddressOfFunctions);
	WORD* addressOfNameOrdinals = (WORD*)((BYTE*)hModule + exportDirectory->AddressOfNameOrdinals);
	DWORD* addressOfNames = (DWORD*)((BYTE*)hModule + exportDirectory->AddressOfNames);
	for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
		if (__strcmp(lpProcName, (const char*)hModule + addressOfNames[i]) == 0) {
			return (FARPROC)((BYTE*)hModule + addressOfFunctions[addressOfNameOrdinals[i]]);
		}
	}
	return NULL;
}