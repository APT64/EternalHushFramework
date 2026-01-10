#include <Windows.h>
#include <config.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
extern CONFIG g_conf;
int g_timeout;
SOCKET initialize_connection_provider() {
	struct sockaddr_in sin;
	g_timeout = 0;

	SOCKET s_socket, c_socket;
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData = { 0 };
	WSAStartup(wVersionRequested, &wsaData);

	s_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_socket == INVALID_SOCKET) {
		return 0 ;
	}

	sin.sin_family = AF_INET;
	sin.sin_port = htons(g_conf.port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s_socket, (struct sockaddr*)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		return 0;
	}

	if (listen(s_socket, SOMAXCONN) == SOCKET_ERROR)
	{
		return 0;
	}
	return s_socket;
/*
	while (true)
	{
		HANDLE client_thread;
		struct sockaddr_in client;
		int client_size = sizeof(client);
		SOCKET client_socket = accept(s_socket, (struct sockaddr*)&client, &client_size);
		if (client_socket == SOCKET_ERROR)
		{
			return;
		}
		int sndsize = 0x10000;
		auto err = setsockopt(client_socket, SOL_SOCKET, SO_SNDBUF, (char*)&sndsize, (int)sizeof(sndsize));
		err = setsockopt(client_socket, SOL_SOCKET, SO_RCVBUF, (char*)&sndsize, (int)sizeof(sndsize));
		client_thread = CreateThread(NULL, 1024 * 1024, (LPTHREAD_START_ROUTINE)main_dispatcher, (LPVOID)client_socket, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	}*/
}