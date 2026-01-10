#define TCP_CONNECTION 2
#define UDP_CONNECTION 4
#define HTTP_CONNECTION 8

#include <Windows.h>
#include <vector>
#include <string>
#include <Python.h>

typedef struct handler
{
	char* main_name;	
	char* module_name;
};

class Connection {
public:
	SOCKET socket;
	ULONG type;
	ULONG id;
	char* connection_addr;
	std::vector<handler> handler_list;
};