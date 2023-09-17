#include <Windows.h>
#include <Connection.h>

class TcpClient {
public:
	TcpClient::TcpClient(Connection* c) {
		connection = c;
	}
	bool setup(int port, unsigned char* host);
	int datasend(LPVOID data, ULONG size);
	int datarecv(LPVOID data, ULONG size);

private:
	Connection *connection;
	sockaddr_in client;
};