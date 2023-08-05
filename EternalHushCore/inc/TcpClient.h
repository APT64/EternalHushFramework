#include <Windows.h>

class TcpClient {
public:

	void setup(int port);
	void datasend(LPVOID data, ULONG size);
	void end();

private:
	BOOL terminate = false;
	sockaddr_in client;
	SOCKET sock;
};