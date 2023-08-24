#include <TcpServer.h>
#include <Windows.h>

void TcpServer::setup(int port) {

	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData = { 0 };
	WSAStartup(wVersionRequested, &wsaData);

	s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_socket == INVALID_SOCKET) {
		return;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s_socket, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		return;
	}

	if (listen(s_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return;
	}
}

void TcpServer::start() {

	client_thread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)callback, this, NULL, NULL);
}