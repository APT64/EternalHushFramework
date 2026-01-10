#include <Windows.h>

void main_dispatcher(SOCKET hConnect);
void invoke_dispatch(SOCKET s) {
	main_dispatcher(s);
}