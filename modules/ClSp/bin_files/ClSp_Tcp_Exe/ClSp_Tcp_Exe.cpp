#include <Windows.h>
#include "ClSp_executable.h"

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	start_executable();
	MessageBoxA(0, 0, 0, 0);
}