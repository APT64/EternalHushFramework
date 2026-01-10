#include <Windows.h>
#include <vector>
#include <pipe_executor.h>
#include <config.h>
#include <syslib.hpp>
#include <module_context.h>

typedef struct {
	HANDLE std_write;
	HANDLE std_read;
} RPROCESS;

NTSTATUS handle_cmd(PMODULE_CONTEXT ctx);

#define PACK_GLE_STATUS	  builder.add_byte(bResult); \
						  builder.add_int(GetLastError());

#define PACK_NTSTATUS_STATUS	  builder.add_byte(bResult); \
						  builder.add_int(status);