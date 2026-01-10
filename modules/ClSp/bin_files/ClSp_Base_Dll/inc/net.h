#include <Windows.h>
#include <winhttp.h>
#include <vector>
#include <iostream>
#include <resp_builder.hpp>
#include <module_context.h>

//void send_encrypted_taskresponse(VOID* pctx, char* buffer, int len);
//void send_encrypted_taskresponse_big(VOID *pctx, std::vector<UCHAR> buffer, ResponseBuilder* builder);
/*
void send_encrypted_chunked(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, std::vector<UCHAR> buffer, ResponseBuilder* builder, std::wstring whdr);
void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, char* buffer, int& len, std::wstring hdr);
void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, std::vector<UCHAR>& buffer, std::wstring hdr);
void send_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, char* buffer, int len, std::wstring hdr);


void send_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);
void send_encrypted_sized(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len);
void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, SOCKET s, char* buffer, int len, int flag);

void _send(HINTERNET s, char* buffer, int len, std::wstring hdr);
void _send(HINTERNET s, char* buffer, int len);
void _recv(HINTERNET s, char* buffer, int& len);
void _recv(HINTERNET s, char* buffer, int& len, std::wstring hdr);
void _recv(HINTERNET s, std::vector<UCHAR>& buffer, std::wstring hdr);
*/