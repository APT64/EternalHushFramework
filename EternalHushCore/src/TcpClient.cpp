#include <TcpClient.h>

void TcpClient::setup(int port) {
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData = { 0 };

	WSAStartup(wVersionRequested, &wsaData);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return;
	}

	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(sock, (sockaddr*)&client, sizeof(client));
}

void TcpClient::datasend(LPVOID data, ULONG size) {
	send(sock, (const char*)data, size, 0);
}

void TcpClient::end() {
	shutdown(sock, 2);
	
}