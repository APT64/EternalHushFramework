#include <dana.hpp>

#pragma pack(push, 1)
typedef struct
{
	ULONG64 PhysicalAddress;
	UCHAR Data[8];
}TS_WRITE_PMEM;
#pragma pack(pop)

class ThrottleStopLayer : public DaNaBaseClass {
private:
	HANDLE dev_handle;
	const std::wstring dev_name = L"\\\\.\\ThrottleStop";
	const DWORD write_pmem_ioctl = 0x8000649C;
	const DWORD read_pmem_ioctl = 0x80006498;

public:
	NTSTATUS Initialize();
	NTSTATUS Finalize();

	NTSTATUS ReadVirtualMemory(PVOID64 memptr, PVOID64 out_buffer, LONG64 size);
	NTSTATUS WriteVirtualMemory(PVOID64 input_buffer, PVOID64 memptr, LONG64 size);

	NTSTATUS ReadPhysicalMemory(PVOID64 memptr, PVOID64 out_buffer, LONG64 size);
	NTSTATUS WritePhysicalMemory(PVOID64 input_buffer, PVOID64 memptr, LONG64 size);
};