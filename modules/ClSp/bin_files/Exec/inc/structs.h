#include <Windows.h>
#include <vector>

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
	USHORT output_length;
	char output[4094];
}OUTPUT, * POUTPUT;