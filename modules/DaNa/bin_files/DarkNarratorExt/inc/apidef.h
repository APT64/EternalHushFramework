#include <Windows.h>
#include <iostream>
#include <vector>
#include <cmd_parser.hpp>

NTSTATUS SelectLayer(PMODULE_CONTEXT pCtx, CommandParser* parser, ResponseBuilder* builder);
NTSTATUS FinalizeLayer(PMODULE_CONTEXT pCtx, CommandParser* parser, ResponseBuilder* builder);
NTSTATUS ReadPhysMem(PMODULE_CONTEXT pCtx, CommandParser* parser, ResponseBuilder* builder);
NTSTATUS WritePhysMem(PMODULE_CONTEXT pCtx, CommandParser* parser, ResponseBuilder* builder);
NTSTATUS ReadVirtMem(PMODULE_CONTEXT pCtx, CommandParser* parser, ResponseBuilder* builder);
NTSTATUS WriteVirtMem(PMODULE_CONTEXT pCtx, CommandParser* parser, ResponseBuilder* builder);
NTSTATUS LeakKernelOb(PMODULE_CONTEXT pCtx, CommandParser* parser, ResponseBuilder* builder);

