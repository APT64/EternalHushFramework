#ifndef SYSLIB
#define SYSLIB

#include <Windows.h>
#include <vector>
#include <hashtable.h>
#include <mem_ldr.h>
#include <vector>
#include <iostream>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) == 0)
//#define STATUS_SUCCESS   ((NTSTATUS)0x00000000L)
#define true 1
#define MAX_STACK_SIZE 12000
#define RBP_OP_INFO 0x5
#define UNDEF_SYSCALL_PARAM64 NULL
#define MEMBEROFOBJECT __declspec(noinline)

extern "C" PVOID NTAPI spoofcall(PVOID a, ...);
extern "C" NTSTATUS NTAPI nt32call(WORD ssn, PVOID funcptr, int argc, ...);

typedef struct
{
    PVOID       Fixup;             // 0
    PVOID       OG_retaddr;        // 8
    PVOID       rbx;               // 16
    PVOID       rdi;               // 24
    PVOID       BTIT_ss;           // 32
    PVOID       BTIT_retaddr;      // 40
    PVOID       Gadget_ss;         // 48
    PVOID       RUTS_ss;           // 56
    PVOID       RUTS_retaddr;      // 64
    PVOID       ssn;               // 72  
    PVOID       trampoline;        // 80
    PVOID       rsi;               // 88
    PVOID       r12;               // 96
    PVOID       r13;               // 104
    PVOID       r14;               // 112
    PVOID       r15;               // 120
} PRM, * PPRM;

/* God Bless Vulcan Raven*/
typedef struct
{
    LPCWSTR dllPath;
    ULONG offset;
    ULONG totalStackSize;
    BOOL requiresLoadLibrary;
    BOOL setsFramePointer;
    PVOID returnAddress;
    BOOL pushRbp;
    ULONG countOfCodes;
    BOOL pushRbpIndex;
} StackFrame, * PStackFrame;

typedef enum _UNWIND_OP_CODES {
    UWOP_PUSH_NONVOL = 0, /* info == register number */
    UWOP_ALLOC_LARGE,     /* no info, alloc size in next 2 slots */
    UWOP_ALLOC_SMALL,     /* info == size of allocation / 8 - 1 */
    UWOP_SET_FPREG,       /* no info, FP = RSP + UNWIND_INFO.FPRegOffset*16 */
    UWOP_SAVE_NONVOL,     /* info == register number, offset in next slot */
    UWOP_SAVE_NONVOL_FAR, /* info == register number, offset in next 2 slots */
    UWOP_SAVE_XMM128 = 8, /* info == XMM reg number, offset in next slot */
    UWOP_SAVE_XMM128_FAR, /* info == XMM reg number, offset in next 2 slots */
    UWOP_PUSH_MACHFRAME   /* info == 0: no error-code, 1: error-code */
} UNWIND_CODE_OPS;

typedef union _UNWIND_CODE {
    struct {
        BYTE CodeOffset;
        BYTE UnwindOp : 4;
        BYTE OpInfo : 4;
    };
    USHORT FrameOffset;
} UNWIND_CODE, * PUNWIND_CODE;

typedef struct _UNWIND_INFO {
    BYTE Version : 3;
    BYTE Flags : 5;
    BYTE SizeOfProlog;
    BYTE CountOfCodes;
    BYTE FrameRegister : 4;
    BYTE FrameOffset : 4;
    UNWIND_CODE UnwindCode[1];
    /*  UNWIND_CODE MoreUnwindCode[((CountOfCodes + 1) & ~1) - 1];
    *   union {
    *       OPTIONAL ULONG ExceptionHandler;
    *       OPTIONAL ULONG FunctionEntry;
    *   };
    *   OPTIONAL ULONG ExceptionData[]; */
} UNWIND_INFO, * PUNWIND_INFO;

typedef struct _SYSCALL_ENTRY {
    DWORD32 hash;
    WORD code;
} SYSCALL_ENTRY;

#ifdef _WIN64
#define DEVICE_NAME L"\\KnownDlls\\ntdll.dll"
#define SSN_OFFFSET 0x4
#else
#define DEVICE_NAME L"\\KnownDlls\\ntdll.dll"
#define SSN_OFFFSET 0x1
#endif

template<std::size_t nth, std::size_t... Head, std::size_t... Tail, typename... Types>
constexpr auto remove_nth_element_impl(std::index_sequence<Head...>, std::index_sequence<Tail...>, std::tuple<Types...> const& tup) {
    return std::tuple{
        std::get<Head>(tup)...,
        // We +1 to refer one element after the one removed 
        std::get<Tail + nth + 1>(tup)...
    };
}

template<std::size_t nth, typename... Types>
constexpr auto remove_nth_element(std::tuple<Types...> const& tup) {
    return remove_nth_element_impl<nth>(
        std::make_index_sequence<nth>(), // We -1 to drop one element 
        std::make_index_sequence<sizeof...(Types) - nth - 1>(),
        tup
        );
}

template <typename... Ts>
auto tuple_to_pvoid(const std::tuple<Ts...>& t) {
    return std::apply([](const auto&... elem) {
        return std::make_tuple((INT64)(elem)...);
        }, t);
}


class syslib
{
public:
    syslib();
    ~syslib();
    MEMBEROFOBJECT NTSTATUS nt_call_0arg(DWORD32 hash);
    MEMBEROFOBJECT NTSTATUS nt_call_1arg(DWORD32 hash, PVOID a1);
    MEMBEROFOBJECT NTSTATUS nt_call_2arg(DWORD32 hash, PVOID a1, PVOID a2);
    MEMBEROFOBJECT NTSTATUS nt_call_3arg(DWORD32 hash, PVOID a1, PVOID a2, PVOID a3);
    void initialize_table();

    template<typename... Args>
    MEMBEROFOBJECT NTSTATUS nt_call(DWORD32 hash, Args&&... args) {
#ifdef _WIN64
        PRM p = { 0 };
        PVOID ntdll_ptr = MemoryGetModuleHandle(WHASH_ntdll);
        PrepareParamStruct(&p, hash);
        
        PVOID funcptr = MemoryGetProcAddressP((PUCHAR)ntdll_ptr, hash);
        char a[512];

        //sprintf(a, "ntdll_ptr %p\nfuncptr %p\nssn %x\nBTIT_retaddr %p\nRUTS_retaddr %p\nBTIT_ss %p\nRUTS_ss %p\ntrampoline %p\ngadget_ss %p\nfixup %p\nog_retaddr %p", ntdll_ptr, funcptr, p.ssn, p.BTIT_retaddr, p.RUTS_retaddr, p.BTIT_ss, p.RUTS_ss, p.trampoline, p.Gadget_ss,p.Fixup, p.OG_retaddr);
        //OutputDebugStringA(a);
        auto n = tuple_to_pvoid(std::make_tuple(args...));
        PVOID a1 = (PVOID)std::get<0>(n);
        PVOID a2 = (PVOID)std::get<1>(n);
        PVOID a3 = (PVOID)std::get<2>(n);
        PVOID a4 = (PVOID)std::get<3>(n);

        auto _new_args = remove_nth_element<0>(remove_nth_element<0>(remove_nth_element<0>(remove_nth_element<0>(n))));
        auto fp = std::make_tuple((PVOID)a1, (PVOID)a2, (PVOID)a3, (PVOID)a4, (PVOID)&p, (PVOID)funcptr, (PVOID)(sizeof(_new_args) / sizeof(PVOID)));
        auto final_tuple = std::tuple_cat(fp, _new_args);
        return (NTSTATUS)std::apply(spoofcall, final_tuple);
#else
        PVOID ntdll_ptr = MemoryGetModuleHandle(WHASH_ntdll);
        WORD ssn = syslib::get_ssn_from_table(hash);
        PVOID funcptr = MemoryGetProcAddressP((PUCHAR)ntdll_ptr, hash);
        auto final_tuple = std::make_tuple((WORD)ssn, (PVOID)funcptr, (int)sizeof...(args), std::forward<Args>(args)...);
        return (NTSTATUS)std::apply(nt32call, final_tuple);
#endif
    }

private:
    std::vector<SYSCALL_ENTRY> entrylist;
    DWORD32 get_ssn_from_table(DWORD32 hash);
    BOOL get_fresh_ntdll();
    HANDLE ntdll_section = 0;
    PVOID fresh_ntdll_ptr = 0;

#ifdef _WIN64
    ULONG CalculateFunctionStackSizeWrapper(PVOID ReturnAddress);
    PVOID FindGadget(LPBYTE Module, ULONG Size);
    ULONG CalculateFunctionStackSize(PRUNTIME_FUNCTION pRuntimeFunction, const DWORD64 ImageBase);
    VOID PrepareParamStruct(PPRM p, DWORD hash);
#else

#endif
};
#endif