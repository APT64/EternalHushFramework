#pragma once
#include <stdio.h>
#pragma once
#include <windows.h>



class TcpServer {
public:
	void setup(int port);
	void start();

	PVOID callback;
	sockaddr_in c_sin;
	SOCKET c_socket, s_socket;;
	BOOL terminate = false;

private:
	HANDLE client_thread;
	struct sockaddr_in sin;
	
};