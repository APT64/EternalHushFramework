#include <WS2tcpip.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iphlpapi.h>
#include <iostream>

char* _inet_ntoa(char* s);
const char* lookup_iface_addr(uint32_t idx);
uint32_t lookup_iface_metric(uint32_t idx);