#include <net.h>
#include <AES.h>

void send_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len) {
	AES aes(AESKeyLength::AES_256);
	std::vector<UCHAR> vec_data(buffer, buffer + len);
	auto data = aes.EncryptCBC(vec_data, key, iv);
	send(s, (const char*)data.data(), data.size(), 0);
	return;
}

void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len) {
	char* buf = new char[len];
	if (recv(s, buf, len, 0) > 0)
	{
		AES aes(AESKeyLength::AES_256);
		std::vector<UCHAR> vec_data(buf, buf + len);
		auto data = aes.DecryptCBC(vec_data, key, iv);
		buffer = (char*)data.data();
	}
	return;
}