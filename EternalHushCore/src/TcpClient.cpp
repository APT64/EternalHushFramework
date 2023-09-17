#include <TcpClient.h>
#include <Python.h>

bool TcpClient::setup(int port, unsigned char* host) {
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData = { 0 };

	WSAStartup(wVersionRequested, &wsaData);

	connection->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (connection->socket == INVALID_SOCKET) {
		PyErr_SetString(PyExc_ConnectionError, "Failed to create TCP socket");
		PyErr_Print();
		return false;
	}
	
	client.sin_family = AF_INET;
	client.sin_port = htons(port);
	client.sin_addr.s_addr = inet_addr((const char*)host);

	connection->type = TCP_CONNECTION;

	connection->connection_addr = (char*)host;
	if (connect(connection->socket, (sockaddr*)&client, sizeof(client)) == SOCKET_ERROR)
	{
		PyErr_SetString(PyExc_ConnectionRefusedError, "Connection refused");
		PyErr_Print();
		return false;
	}
	return true;
}

int TcpClient::datasend(LPVOID data, ULONG size) {
	int send_bytes = send(connection->socket, (const char*)data, size, 0);
	if (send_bytes == SOCKET_ERROR) 
	{
		PyErr_SetString(PyExc_ConnectionError, "Failed to send data");
		PyErr_Print();
	}
	return send_bytes;
}

int TcpClient::datarecv(LPVOID data, ULONG size) {
	int recv_bytes = recv(connection->socket, (char*)data, size, 0);
	if (recv_bytes == SOCKET_ERROR)
	{
		PyErr_SetString(PyExc_ConnectionError, "Failed to receive data");
		PyErr_Print();
	}
	return recv_bytes;
}