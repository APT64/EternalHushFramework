#include <iostream>
#include <windows.h>
#include <vector>
#include <module_context.h>
#include <extapi.h>
#include <darknarrator.h>
#include <cmd_parser.hpp>
#include <apidef.h>
#include <global.h>

PMODULE_CONTEXT pCtx;
static EXTENSION_APIENTRY api_set[] = {
	{DANA_READPHYSMEM_UID, (LONGLONG)ReadPhysMem},
	{DANA_WRITEPHYSMEM_UID, (LONGLONG)WritePhysMem},
	{DANA_READVIRTMEM_UID, (LONGLONG)ReadVirtMem},
	{DANA_WRITEVIRTMEM_UID, (LONGLONG)WriteVirtMem},
	{DANA_SELECT_LAYER_UID, (LONGLONG)SelectLayer},
	{DANA_FINALIZE_LAYER_UID, (LONGLONG)FinalizeLayer},
	{DANA_LEAK_OBJECT_UID, (LONGLONG)LeakKernelOb},
	{DANA_LEAK_MODULE_UID, (LONGLONG)LeakModuleAddr},
};

extern "C" __declspec(dllexport) NTSTATUS RegisterExt(PMODULE_CONTEXT ctx, PEXTENSION_OBJECT ext_object) {
	pCtx = ctx;
	
	ext_object->ext_uid = DARKNARRATOR_UID;
	ext_object->api_table = std::vector<EXTENSION_APIENTRY>(std::begin(api_set), std::end(api_set));
	

	return ERROR_SUCCESS;

}