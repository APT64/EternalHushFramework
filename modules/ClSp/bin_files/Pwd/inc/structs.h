#include <Windows.h>
#include <vector>
#include <string>

typedef VOID(*RECVENCRYPTED)(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);
typedef VOID(*SENDENCRYPTED)(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);

typedef struct {
	SOCKET connection;
	std::vector<UCHAR> aes_key;
	std::vector<UCHAR> iv_key;
	RECVENCRYPTED recv_encrypted;
	SENDENCRYPTED send_encrypted;
	std::vector<std::string> argv;
}MODULE_CONTEXT, * PMODULE_CONTEXT;

typedef struct {
	USHORT path_length;
	char path[254];
}PATH, * PPATH;