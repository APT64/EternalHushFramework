#include <Windows.h>

class TcpClient {
public:

	void setup(int port, unsigned char* host);
	void datasend(LPVOID data, ULONG size);
	int datarecv(LPVOID data, ULONG size);
	void end();
	SOCKET sock;
	int id;
private:
	BOOL terminate = false;
	sockaddr_in client;
};