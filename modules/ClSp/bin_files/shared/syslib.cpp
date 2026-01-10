#include <syslib.hpp>
#include <vector>
#include <cutils.h>
#include <mem_ldr.h>
#include <intstructs.h>
#include <hashtable.h>
#define uint64_t unsigned long long

#ifdef _WIN64
VOID syslib::PrepareParamStruct(PPRM p, DWORD hash) {
    PVOID ReturnAddress = NULL;
    NTSTATUS status = 0;
    PVOID kernel32_ptr = MemoryGetModuleHandle(WHASH_kernel32);
    PVOID ntdll_ptr = MemoryGetModuleHandle(WHASH_ntdll);
    p->trampoline = syslib::FindGadget((LPBYTE)ntdll_ptr, 0x200000);
    ReturnAddress = (PBYTE)(MemoryGetProcAddressP((PUCHAR)kernel32_ptr, HASH_BaseThreadInitThunk)) + 0x14;
    p->BTIT_ss = (PVOID)syslib::CalculateFunctionStackSizeWrapper(ReturnAddress);
    p->BTIT_retaddr = ReturnAddress;
    ReturnAddress = (PBYTE)(MemoryGetProcAddressP((PUCHAR)ntdll_ptr, HASH_RtlUserThreadStart)) + 0x21;
    p->RUTS_ss = (PVOID)syslib::CalculateFunctionStackSizeWrapper(ReturnAddress);
    p->RUTS_retaddr = ReturnAddress;
    p->Gadget_ss = (PVOID)syslib::CalculateFunctionStackSizeWrapper(p->trampoline);
    DWORD ssn = syslib::get_ssn_from_table(hash);
    p->ssn = (PVOID)ssn;
    return;
}
PVOID syslib::FindGadget(LPBYTE Module, ULONG Size)
{
    for (int x = 0; x < Size; x++)
    {
        if (memcmp(Module + x, "\xFF\x23", 2) == 0)
        {
            return (PVOID)(Module + x);
        };
    };

    return NULL;
}
ULONG syslib::CalculateFunctionStackSize(PRUNTIME_FUNCTION pRuntimeFunction, const DWORD64 ImageBase)
{
    NTSTATUS status = 0;
    PUNWIND_INFO pUnwindInfo = NULL;
    ULONG unwindOperation = 0;
    ULONG operationInfo = 0;
    ULONG index = 0;
    ULONG frameOffset = 0;
    StackFrame stackFrame = { 0 };


    // [0] Sanity check incoming pointer.
    if (!pRuntimeFunction)
    {
        status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    // [1] Loop over unwind info.
    // NB As this is a PoC, it does not handle every unwind operation, but
    // rather the minimum set required to successfully mimic the default
    // call stacks included.
    pUnwindInfo = (PUNWIND_INFO)(pRuntimeFunction->UnwindData + ImageBase);
    while (index < pUnwindInfo->CountOfCodes)
    {
        unwindOperation = pUnwindInfo->UnwindCode[index].UnwindOp;
        operationInfo = pUnwindInfo->UnwindCode[index].OpInfo;
        // [2] Loop over unwind codes and calculate
        // total stack space used by target Function.
        switch (unwindOperation) {
        case UWOP_PUSH_NONVOL:
            // UWOP_PUSH_NONVOL is 8 bytes.
            stackFrame.totalStackSize += 8;
            // Record if it pushes rbp as
            // this is important for UWOP_SET_FPREG.
            if (RBP_OP_INFO == operationInfo)
            {
                stackFrame.pushRbp = true;
                // Record when rbp is pushed to stack.
                stackFrame.countOfCodes = pUnwindInfo->CountOfCodes;
                stackFrame.pushRbpIndex = index + 1;
            }
            break;
        case UWOP_SAVE_NONVOL:
            //UWOP_SAVE_NONVOL doesn't contribute to stack size
            // but you do need to increment index.
            index += 1;
            break;
        case UWOP_ALLOC_SMALL:
            //Alloc size is op info field * 8 + 8.
            stackFrame.totalStackSize += ((operationInfo * 8) + 8);
            break;
        case UWOP_ALLOC_LARGE:
            // Alloc large is either:
            // 1) If op info == 0 then size of alloc / 8
            // is in the next slot (i.e. index += 1).
            // 2) If op info == 1 then size is in next
            // two slots.
            index += 1;
            frameOffset = pUnwindInfo->UnwindCode[index].FrameOffset;
            if (operationInfo == 0)
            {
                frameOffset *= 8;
            }
            else
            {
                index += 1;
                frameOffset += (pUnwindInfo->UnwindCode[index].FrameOffset << 16);
            }
            stackFrame.totalStackSize += frameOffset;
            break;
        case UWOP_SET_FPREG:
            // This sets rsp == rbp (mov rsp,rbp), so we need to ensure
            // that rbp is the expected value (in the frame above) when
            // it comes to spoof this frame in order to ensure the
            // call stack is correctly unwound.
            stackFrame.setsFramePointer = true;
            break;
        default:
            status = STATUS_ASSERTION_FAILURE;
            break;
        }

        index += 1;
    }

    // If chained unwind information is present then we need to
    // also recursively parse this and add to total stack size.
    if (0 != (pUnwindInfo->Flags & UNW_FLAG_CHAININFO))
    {
        index = pUnwindInfo->CountOfCodes;
        if (0 != (index & 1))
        {
            index += 1;
        }
        pRuntimeFunction = (PRUNTIME_FUNCTION)(&pUnwindInfo->UnwindCode[index]);
        return CalculateFunctionStackSize(pRuntimeFunction, ImageBase);
    }

    // Add the size of the return address (8 bytes).
    stackFrame.totalStackSize += 8;

    return stackFrame.totalStackSize;
Cleanup:
    return status;
}
ULONG syslib::CalculateFunctionStackSizeWrapper(PVOID ReturnAddress)
{
    NTSTATUS status = 0;
    PRUNTIME_FUNCTION pRuntimeFunction = NULL;
    DWORD64 ImageBase = 0;
    PUNWIND_HISTORY_TABLE pHistoryTable = NULL;

    // [0] Sanity check return address.
    if (!ReturnAddress)
    {
        status = STATUS_INVALID_PARAMETER;
        goto Cleanup;
    }

    // [1] Locate RUNTIME_FUNCTION for given Function.
    pRuntimeFunction = RtlLookupFunctionEntry((DWORD64)ReturnAddress, &ImageBase, pHistoryTable);
    if (NULL == pRuntimeFunction)
    {
        status = STATUS_ASSERTION_FAILURE;
        goto Cleanup;
    }

    // [2] Recursively calculate the total stack size for
    // the Function we are "returning" to.
    return CalculateFunctionStackSize(pRuntimeFunction, ImageBase);

Cleanup:
    return status;
}
#endif
syslib::syslib() {
}
syslib::~syslib() {
}
NTSTATUS syslib::nt_call_0arg(DWORD32 hash) {
#ifdef _WIN64
    return syslib::nt_call(hash, UNDEF_SYSCALL_PARAM64, UNDEF_SYSCALL_PARAM64, UNDEF_SYSCALL_PARAM64, UNDEF_SYSCALL_PARAM64);
#else
    return syslib::nt_call(hash);
#endif
}
NTSTATUS syslib::nt_call_1arg(DWORD32 hash, PVOID a1) {
#ifdef _WIN64
    return syslib::nt_call(hash, a1, UNDEF_SYSCALL_PARAM64, UNDEF_SYSCALL_PARAM64, UNDEF_SYSCALL_PARAM64);
#else
    return syslib::nt_call(hash, a1);
#endif
}
NTSTATUS syslib::nt_call_2arg(DWORD32 hash, PVOID a1, PVOID a2) {
#ifdef _WIN64
    return syslib::nt_call(hash, a1, a2, UNDEF_SYSCALL_PARAM64, UNDEF_SYSCALL_PARAM64);
#else
    return syslib::nt_call(hash, a1, a2);
#endif
}
NTSTATUS syslib::nt_call_3arg(DWORD32 hash, PVOID a1, PVOID a2, PVOID a3) {
#ifdef _WIN64
    return syslib::nt_call(hash, a1, a2, a3, UNDEF_SYSCALL_PARAM64);
#else
    return syslib::nt_call(hash, a1, a2, a3);
#endif
}

BOOL syslib::get_fresh_ntdll() {
    PUCHAR pCurrentNtdll = (PUCHAR)MemoryGetModuleHandle(WHASH_ntdll);
    DWORD64 ntdll_size = MemoryGetModuleSize(WHASH_ntdll);
    using fnNtOpenSection = NTSTATUS(NTAPI*)(PHANDLE SectionHandle, ACCESS_MASK DesiredAccess, OBJECT_ATTRIBUTES* ObjectAttributes);
    using fnNtMapViewOfSection = NTSTATUS(NTAPI*)(HANDLE SectionHandle, HANDLE ProcessHandle, PVOID* BaseAddress, ULONG_PTR ZeroBits, SIZE_T CommitSize, PLARGE_INTEGER SectionOffset, PSIZE_T ViewSize, DWORD InheritDisposition, ULONG AllocationType, ULONG Win32Protect);

    fnNtOpenSection pNtOpenSection = (fnNtOpenSection)MemoryGetProcAddressP(pCurrentNtdll, HASH_NtOpenSection);
    fnNtMapViewOfSection pNtMapViewOfSection = (fnNtMapViewOfSection)MemoryGetProcAddressP(pCurrentNtdll, HASH_NtMapViewOfSection);

    LARGE_INTEGER large_ntdll_sz = { ntdll_size };
    OBJECT_ATTRIBUTES ntdll_objattr = { 0 };

    UNICODE_STRING device_name = UNICODE_STRING{ sizeof(DEVICE_NAME) - sizeof(WCHAR), sizeof(DEVICE_NAME), (PWSTR)DEVICE_NAME };
    InitializeObjectAttributes(&ntdll_objattr, &device_name, 0x00000040, 0, 0);

    NTSTATUS status = pNtOpenSection(&(syslib::ntdll_section), SECTION_MAP_READ | SECTION_MAP_WRITE, (OBJECT_ATTRIBUTES*)&ntdll_objattr);
    status = pNtMapViewOfSection(syslib::ntdll_section, (HANDLE)(-1), &(syslib::fresh_ntdll_ptr), NULL, NULL, NULL, (PSIZE_T)&ntdll_size, 2, NULL, PAGE_READWRITE);
    if (syslib::fresh_ntdll_ptr != 0) return TRUE;
    return FALSE;
}
void syslib::initialize_table() {
    BOOL bResult = syslib::get_fresh_ntdll();
    if (!bResult) return;

    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)syslib::fresh_ntdll_ptr;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)syslib::fresh_ntdll_ptr + dosHeader->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)syslib::fresh_ntdll_ptr +
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    DWORD* addressOfFunctions = (DWORD*)((BYTE*)syslib::fresh_ntdll_ptr + exportDirectory->AddressOfFunctions);
    WORD* addressOfNameOrdinals = (WORD*)((BYTE*)syslib::fresh_ntdll_ptr + exportDirectory->AddressOfNameOrdinals);
    DWORD* addressOfNames = (DWORD*)((BYTE*)syslib::fresh_ntdll_ptr + exportDirectory->AddressOfNames);
    for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
        PCHAR name = (char*)syslib::fresh_ntdll_ptr + addressOfNames[i];
        if (*name == 'N' &&
            *(name + 1) == 't' &&
            *(name + 2) >= 65 &&
            *(name + 2) <= 90
            )
        {
            DWORD32 hash = default_hashfunc(name, _strlen(name));
            PVOID pFuncProto = (FARPROC)((BYTE*)syslib::fresh_ntdll_ptr + addressOfFunctions[addressOfNameOrdinals[i]]);
            WORD ssn = *(PWORD)((PUCHAR)pFuncProto + SSN_OFFFSET);

            syslib::entrylist.push_back({ hash, ssn });
        }
    }
    PUCHAR pCurrentNtdll = (PUCHAR)MemoryGetModuleHandle(WHASH_ntdll);
    using fnNtUnmapViewOfSection = NTSTATUS(NTAPI*)(HANDLE ProcessHandle, PVOID BaseAddress);
    fnNtUnmapViewOfSection pNtUnmapViewOfSection = (fnNtUnmapViewOfSection)MemoryGetProcAddressP(pCurrentNtdll, HASH_NtUnmapViewOfSection);

    this->nt_call_2arg(HASH_NtUnmapViewOfSection, (HANDLE)(-1), syslib::fresh_ntdll_ptr);
    this->nt_call_1arg(HASH_NtClose, syslib::ntdll_section);
}

DWORD32 syslib::get_ssn_from_table(DWORD32 hash) {
    for (int i = 0; i < syslib::entrylist.size(); i++) {
        if (syslib::entrylist[i].hash == hash) { return syslib::entrylist[i].code; }
    }
    return 0;
}

