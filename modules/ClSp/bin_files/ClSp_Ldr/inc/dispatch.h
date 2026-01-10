#include <Windows.h>
#include <config.h>
#include <vector>
#include <AES.h>
#include <module_context.h>

volatile NTSTATUS RemoteCall(PMODULE_CONTEXT ctx);