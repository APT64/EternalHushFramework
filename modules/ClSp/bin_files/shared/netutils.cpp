#include <WS2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iphlpapi.h>
#include <iostream>
#pragma comment(lib, "iphlpapi.lib")
char*
_inet_ntoa(char* s)
{
    char* buffer = new char[18];
    unsigned char* bytes = (unsigned char*)s;
    snprintf(buffer, 18, "%d.%d.%d.%d",
        bytes[0], bytes[1], bytes[2], bytes[3]);

    return buffer;
}

const char*
lookup_iface_addr(uint32_t idx) {
    PMIB_IPADDRTABLE pIpTable = 0;
    DWORD szIpTable = 0;
    if (!GetIpAddrTable(pIpTable, &szIpTable, TRUE)) return "";
    pIpTable = (PMIB_IPADDRTABLE)new char[szIpTable];
    GetIpAddrTable(pIpTable, &szIpTable, TRUE);
    for (int i = 0; i < pIpTable->dwNumEntries; i++)
    {
        MIB_IPADDRROW* row = &pIpTable->table[i];
        if (row->dwIndex == idx) return _inet_ntoa((char*)&row->dwAddr);
    }
    return "";
}