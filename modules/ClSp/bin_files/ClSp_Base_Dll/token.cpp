#include <Windows.h>
#include <iostream>
#include <sddl.h>

PTOKEN_USER TokenGetUserInfo(HANDLE hToken, DWORD* length) {
	DWORD returnLength;
	if (!GetTokenInformation(hToken, TokenUser, 0, 0, &returnLength)) {
		if (!GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			return 0;
		}
	}
	PTOKEN_USER tokenInfo = (PTOKEN_USER)new char[returnLength];
	*length = returnLength;
	if (!GetTokenInformation(hToken, TokenUser, tokenInfo, returnLength, &returnLength)) {
		delete[] tokenInfo;
		return 0;
	}
	return tokenInfo;
}

std::string TokenGetUID(HANDLE hToken) {
	DWORD tokenLen = 0;
	PTOKEN_USER tokenInfo = TokenGetUserInfo(hToken, &tokenLen);
	if (!tokenInfo)
	{
		return "";
	}
	CHAR name[0x200] = { 0 };
	CHAR domain[0x200] = { 0 };

	DWORD nameLength = sizeof(name);
	DWORD domainLength = sizeof(domain);
	SID_NAME_USE snu;

	// Lookup the account SID to retrieve the username and domain.
	if (!LookupAccountSidA(NULL, tokenInfo->User.Sid, name, &nameLength, domain, &domainLength, &snu)) {
		delete[] tokenInfo;
		return "";
	}
	std::string uid = name;
	uid += "\\";
	uid += domain;
	delete[]tokenInfo;
	return uid;
}
std::string TokenGetSID(HANDLE hToken) {
	DWORD tokenLen = 0;
	PTOKEN_USER tokenInfo = TokenGetUserInfo(hToken, &tokenLen);
	if (!tokenInfo)
	{
		return "";
	}
	LPSTR sid;
	if (!ConvertSidToStringSidA(tokenInfo->User.Sid, &sid)) {
		delete[]tokenInfo;
		return 0;
	}
	delete[]tokenInfo;
	return std::string(sid);

}