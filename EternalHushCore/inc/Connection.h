#define TCP_CONNECTION 2
#define UDP_CONNECTION 4
#define HTTP_CONNECTION 8

#include <Windows.h>

class Connection {
public:
	SOCKET socket;
	ULONG type;
	ULONG id;
	char* connection_addr;
};