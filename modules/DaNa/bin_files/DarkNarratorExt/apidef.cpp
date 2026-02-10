#include <dana.hpp>
#include <cmd_parser.hpp>
//#pragma comment(lib, "ntdll.lib")
#include <superfetch.h>
#include <global.h>
extern PMODULE_CONTEXT pCtx;

DaNaBaseClass* g_layer = 0;

NTSTATUS SelectLayer(PMODULE_CONTEXT ctx, CommandParser* parser, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	pCtx = ctx;

	bool is_layer_exists = false;
	LONGLONG layer_uid = parser->get_long();
	for (int i = 0; i < (sizeof(layer_set) / sizeof(LAYER_ENTRY)); i++)
	{
		if (layer_uid == layer_set[i].layer_uid) {
			is_layer_exists = true;
			g_layer = layer_set[i].layer_object;
			status = g_layer->Initialize();
			if (NT_SUCCESS(status)) builder->add_byte(1);
			else builder->add_byte(0);
		}
	}
	if (!is_layer_exists)
	{
		status = ERROR_NOT_FOUND;
		builder->add_byte(0);
	}

	return status;
}

NTSTATUS FinalizeLayer(PMODULE_CONTEXT ctx, CommandParser* parser, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	pCtx = ctx;

	if (g_layer) { 
		status = g_layer->Finalize();
		if (NT_SUCCESS(status)) builder->add_byte(1);
		else builder->add_byte(0);
	}
	else { 
		status = ERROR_EMPTY;
		builder->add_byte(0);
	}

	return status;
}

NTSTATUS ReadPhysMem(PMODULE_CONTEXT ctx, CommandParser* parser, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	pCtx = ctx;

	LONGLONG memptr, buffer, size = 0;
	memptr = parser->get_long();
	size = parser->get_long();

	buffer = (LONGLONG)new char[size];

	if (g_layer) {
		status = g_layer->ReadPhysicalMemory((PVOID64)memptr, (PVOID64)buffer, size);
		if (NT_SUCCESS(status)) {
			builder->add_bstrarg((PCHAR)buffer, size);
		}
		else
		{
			builder->add_int(0);
		}
	}
	else status = ERROR_INTERNAL_ERROR;

	return status;
}

NTSTATUS WritePhysMem(PMODULE_CONTEXT ctx, CommandParser* parser, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	pCtx = ctx;
	LONGLONG memptr;
	memptr = parser->get_long();
	std::string buffer = parser->get_strarg();

	if (g_layer) {
		status = g_layer->WritePhysicalMemory((PVOID64)buffer.data(), (PVOID64)memptr, buffer.size());
		if (NT_SUCCESS(status)) {
			builder->add_int(buffer.size());
		}
		else{
			builder->add_int(0);
		}
	}
	else status = ERROR_INTERNAL_ERROR;

	return status;
}

NTSTATUS ReadVirtMem(PMODULE_CONTEXT ctx, CommandParser* parser, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	pCtx = ctx;

	LONGLONG memptr, buffer, size = 0;

	memptr = parser->get_long();
	size = parser->get_long();

	buffer = (LONGLONG)new char[size];

	if (g_layer) {
		status = g_layer->ReadVirtualMemory((PVOID64)memptr, (PVOID64)buffer, size);
		if (NT_SUCCESS(status)) {
			builder->add_bstrarg((PCHAR)buffer, size);
		}
		else
		{
			builder->add_int(0);
		}
	}
	else status = ERROR_INTERNAL_ERROR;

	return status;
}

NTSTATUS WriteVirtMem(PMODULE_CONTEXT ctx, CommandParser* parser, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	pCtx = ctx;
	LONGLONG memptr;
	memptr = parser->get_long();
	std::string buffer = parser->get_strarg();

	if (g_layer) {
		status = g_layer->WriteVirtualMemory((PVOID64)buffer.data(), (PVOID64)memptr, buffer.size());
		if (NT_SUCCESS(status)) {
			builder->add_int(buffer.size());
		}
		else {
			builder->add_int(0);
		}
	}
	else status = ERROR_INTERNAL_ERROR;
	return status;
}

NTSTATUS LeakKernelOb(PMODULE_CONTEXT ctx, CommandParser* parser, ResponseBuilder* builder) {
	NTSTATUS status = ERROR_SUCCESS;
	pCtx = ctx;
	LONGLONG handle, process_id, ob_address = 0;
	handle = parser->get_long();
	process_id = parser->get_long();

	if (g_layer) {
		status = g_layer->LeakObjectAddress((HANDLE)handle, process_id, (PVOID64*)&ob_address);
		if (NT_SUCCESS(status)) {
			builder->add_long(ob_address);
		}
	}
	else status = ERROR_INTERNAL_ERROR;
	return status;
	
}