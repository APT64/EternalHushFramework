#include <Windows.h>

void main_dispatcher(SOCKET hConnect);
void invoke_dispatch(SOCKET s) {
	while (true)
	{
		HANDLE client_thread;
		struct sockaddr_in client;
		int client_size = sizeof(client);
		SOCKET client_socket = accept(s, (struct sockaddr*)&client, &client_size);
		if (client_socket == SOCKET_ERROR)
		{
			return;
		}
		int sndsize = 0x10000;
		auto err = setsockopt(client_socket, SOL_SOCKET, SO_SNDBUF, (char*)&sndsize, (int)sizeof(sndsize));
		err = setsockopt(client_socket, SOL_SOCKET, SO_RCVBUF, (char*)&sndsize, (int)sizeof(sndsize));
		client_thread = CreateThread(NULL, 1024 * 1024, (LPTHREAD_START_ROUTINE)main_dispatcher, (LPVOID)client_socket, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
	}
}