#include <Windows.h>
#include <vector>

typedef VOID(*RECVENCRYPTED)(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);
typedef VOID(*SENDENCRYPTED)(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);

void send_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);
void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);