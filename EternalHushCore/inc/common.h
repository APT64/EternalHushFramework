#include <Windows.h>

#define EXPORT extern "C" __declspec(dllexport)
#define FUNC __stdcall

typedef CHAR LSTR;
typedef WCHAR LWSTR;