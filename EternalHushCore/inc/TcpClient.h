#include <Windows.h>

class TcpClient {
public:

	bool setup(int port, unsigned char* host);
	int datasend(LPVOID data, ULONG size);
	int datarecv(LPVOID data, ULONG size);
	void end();
	SOCKET sock;
	int id;
	char* target_addr;
private:
	BOOL terminate = false;
	sockaddr_in client;
};