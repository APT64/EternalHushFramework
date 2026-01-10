import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
from additional import memrwlib
from additional.clingyspider.nt_const import *
import os

def get_proc_baseaddress(pPEB):
    if api.IsX64():
        return memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ImageBaseAddress)
    else:
        return memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ImageBaseAddress)
    
def get_proc_base_heap(pPEB):
    if api.IsX64():
        return memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessHeap)
    else:
        return memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessHeap)
    
def is_process_debugged(pPEB):
    if api.IsX64():
        return bool(memrwlib.ReadByte(pPEB+PEB64_OFFSETLIST.BeingDebugged))
    else:
        return bool(memrwlib.ReadByte(pPEB+PEB32_OFFSETLIST.BeingDebugged))
    
def get_proc_kct(pPEB):
    if api.IsX64():
        return memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.KernelCallbackTable)
    else:
        return memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.KernelCallbackTable)
    
def get_proc_subsystem(pPEB):
    if api.IsX64():
        v = memrwlib.ReadDword(pPEB+PEB64_OFFSETLIST.ImageSubsystem)
    else:
        v = memrwlib.ReadDword(pPEB+PEB32_OFFSETLIST.ImageSubsystem)

    if v == 0:
        return "IMAGE_SUBSYSTEM_UNKNOWN"
    if v == 1:
        return "IMAGE_SUBSYSTEM_NATIVE"
    if v == 2:
        return "IMAGE_SUBSYSTEM_WINDOWS_GUI"
    if v == 3:
        return "IMAGE_SUBSYSTEM_WINDOWS_CUI"
    if v == 5:
        return "IMAGE_SUBSYSTEM_OS2_CUI"
    if v == 7:
        return "IMAGE_SUBSYSTEM_POSIX_CUI"
    if v == 8:
        return "IMAGE_SUBSYSTEM_NATIVE_WINDOWS"
    
def get_proc_nt_globalflag(pPEB):
    if api.IsX64():
        return memrwlib.ReadDword(pPEB+PEB64_OFFSETLIST.NtGlobalFlag)
    else:
        return memrwlib.ReadDword(pPEB+PEB32_OFFSETLIST.NtGlobalFlag)
    
def get_proc_cmdline(pPEB):
    if api.IsX64():
        param_block = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST.CommandLine
        s = memrwlib.ParseUNICODE_STRING64(p)
    else:
        param_block = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST.CommandLine
        s = memrwlib.ParseUNICODE_STRING32(p)

    return s

def get_proc_pathname(pPEB):
    if api.IsX64():
        param_block = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST.ImagePathName
        s = memrwlib.ParseUNICODE_STRING64(p)
    else:
        param_block = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST.ImagePathName
        s = memrwlib.ParseUNICODE_STRING32(p)

    return s

def get_proc_curdir(pPEB):
    if api.IsX64():
        param_block = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST.CurrentDirectory
        s = memrwlib.ParseUNICODE_STRING64(p)
    else:
        param_block = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST.CurrentDirectory
        s = memrwlib.ParseUNICODE_STRING32(p)

    return s

def get_wintitle(pPEB):
    if api.IsX64():
        param_block = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST.WindowTitle
        s = memrwlib.ParseUNICODE_STRING64(p)
    else:
        param_block = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST.WindowTitle
        s = memrwlib.ParseUNICODE_STRING32(p)

    return s

def get_loaded_module_list(pPEB):
    if api.IsX64():
        pLdr = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.Ldr)
        pListHead = pLdr + PEB_LDR_DATA64_OFFSETLIST.InMemoryOrderModuleList
        pEntry = memrwlib.ReadPtr(pListHead+LIST_ENTRY64_OFFSETLIST.Flink)
        while pEntry != pListHead:
            pLdrEntry = pEntry - LIST_ENTRY64_OFFSETLIST.TOTAL_SIZE
            FullName = memrwlib.ParseUNICODE_STRING64(pLdrEntry + LDR_DATA_TABLE_ENTRY64_OFFSETLIST.FullDllName)
            Name = memrwlib.ParseUNICODE_STRING64(pLdrEntry + LDR_DATA_TABLE_ENTRY64_OFFSETLIST.BaseDllName)
            Size = memrwlib.ReadDword(pLdrEntry + LDR_DATA_TABLE_ENTRY64_OFFSETLIST.SizeOfImage)
            EntryPoint = memrwlib.ReadPtr(pLdrEntry + LDR_DATA_TABLE_ENTRY64_OFFSETLIST.DllBase)
            pEntry = memrwlib.ReadPtr(pEntry+LIST_ENTRY64_OFFSETLIST.Flink)
            eh.ui.Echo("{:<20} {:<64} {:<16} {:<8} bytes".format(Name, FullName, hex(EntryPoint), Size), eh.ECHO_DEFAULT)
    else:
        pLdr = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.Ldr)
        pListHead = pLdr + PEB_LDR_DATA32_OFFSETLIST.InMemoryOrderModuleList
        pEntry = memrwlib.ReadPtr(pListHead+LIST_ENTRY32_OFFSETLIST.Flink)
        while pEntry != pListHead:
            pLdrEntry = pEntry - LIST_ENTRY32_OFFSETLIST.TOTAL_SIZE
            FullName = memrwlib.ParseUNICODE_STRING32(pLdrEntry + LDR_DATA_TABLE_ENTRY32_OFFSETLIST.FullDllName)
            Name = memrwlib.ParseUNICODE_STRING32(pLdrEntry + LDR_DATA_TABLE_ENTRY32_OFFSETLIST.BaseDllName)
            Size = memrwlib.ReadDword(pLdrEntry + LDR_DATA_TABLE_ENTRY32_OFFSETLIST.SizeOfImage)
            EntryPoint = memrwlib.ReadPtr(pLdrEntry + LDR_DATA_TABLE_ENTRY32_OFFSETLIST.DllBase)
            pEntry = memrwlib.ReadPtr(pEntry+LIST_ENTRY32_OFFSETLIST.Flink)
            eh.ui.Echo("{:<20} {:<64} {:<8} {:<8} bytes".format(Name, FullName, hex(EntryPoint), Size), eh.ECHO_DEFAULT)

def print_all_process_info(pPEB):
    eh.ui.Echo("Fetching information from PEB...", eh.ECHO_DEFAULT)
    eh.ui.Echo("Process Base Address: {}".format(hex(get_proc_baseaddress(pPEB))), eh.ECHO_DEFAULT)
    if is_process_debugged(pPEB):
        eh.ui.Echo("Debugger Attached: TRUE", eh.ECHO_WARNING)
    else:
        eh.ui.Echo("Debugger Attached: FALSE", eh.ECHO_DEFAULT)
    eh.ui.Echo("Image Subsystem: {}".format(get_proc_subsystem(pPEB)), eh.ECHO_DEFAULT)
    eh.ui.Echo("Process Heap Address: {}".format(hex(get_proc_base_heap(pPEB))), eh.ECHO_DEFAULT)
    eh.ui.Echo("Process KCT Address: {}".format(hex(get_proc_kct(pPEB))), eh.ECHO_DEFAULT)
    eh.ui.Echo("NtGlobalFlag: {}".format(hex(get_proc_nt_globalflag(pPEB))), eh.ECHO_DEFAULT)
    eh.ui.Echo("", eh.ECHO_DEFAULT)
    eh.ui.Echo("Process Parameters:", eh.ECHO_DEFAULT)
    eh.ui.Echo("Command Line: {}".format(get_proc_cmdline(pPEB)), eh.ECHO_DEFAULT)
    eh.ui.Echo("Path Name: {}".format(get_proc_pathname(pPEB)), eh.ECHO_DEFAULT)
    eh.ui.Echo("Current Directory: {}".format(get_proc_curdir(pPEB)), eh.ECHO_DEFAULT)
    eh.ui.Echo("Window Title: {}".format(get_wintitle(pPEB)), eh.ECHO_DEFAULT)
    eh.ui.Echo("", eh.ECHO_DEFAULT)
    eh.ui.Echo("Loaded Modules:", eh.ECHO_DEFAULT)
    get_loaded_module_list(pPEB)

def set_debug_flag(pPEB, flag):
    if api.IsX64():
        memrwlib.WriteByte(pPEB+PEB64_OFFSETLIST.BeingDebugged, flag)
    else:
        memrwlib.WriteByte(pPEB+PEB32_OFFSETLIST.BeingDebugged, flag)

def set_proc_nt_globalflag(pPEB, flag):
    if api.IsX64():
        memrwlib.WriteDword(pPEB+PEB64_OFFSETLIST.NtGlobalFlag, flag)
    else:
        memrwlib.WriteDword(pPEB+PEB32_OFFSETLIST.NtGlobalFlag, flag)

def set_current_dir(pPEB, dir):
    if api.IsX64():
        strptr = memrwlib.AllocUNICODE_STRING64(dir)
        bstr = api.ReadMemory(strptr, UNICODE_STRING64_OFFSETLIST.TOTAL_SIZE)
        param_block = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST.CurrentDirectory
        api.WriteMemory(p, bstr)
    else:
        strptr = memrwlib.AllocUNICODE_STRING32(dir)
        bstr = api.ReadMemory(strptr, UNICODE_STRING32_OFFSETLIST.TOTAL_SIZE)
        param_block = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST.CurrentDirectory
        api.WriteMemory(p, bstr)

def get_first_module_addr(pPEB):
    if api.IsX64():
        pLdr = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.Ldr)
        pListHead = pLdr + PEB_LDR_DATA64_OFFSETLIST.InMemoryOrderModuleList
        pEntry = memrwlib.ReadPtr(pListHead+LIST_ENTRY64_OFFSETLIST.Flink)
        pLdrEntry = pEntry - LIST_ENTRY64_OFFSETLIST.TOTAL_SIZE
    else:
        pLdr = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.Ldr)
        pListHead = pLdr + PEB_LDR_DATA32_OFFSETLIST.InMemoryOrderModuleList
        pEntry = memrwlib.ReadPtr(pListHead+LIST_ENTRY32_OFFSETLIST.Flink)
        pLdrEntry = pEntry - LIST_ENTRY32_OFFSETLIST.TOTAL_SIZE
    return pLdrEntry

def set_proc_cmdline(pPEB, cmdline):
    if api.IsX64():
        strptr = memrwlib.AllocUNICODE_STRING64(cmdline)
        bstr = api.ReadMemory(strptr, UNICODE_STRING64_OFFSETLIST.TOTAL_SIZE)
        param_block = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST.CommandLine
        api.WriteMemory(p, bstr)
    else:
        strptr = memrwlib.AllocUNICODE_STRING32(cmdline)
        bstr = api.ReadMemory(strptr, UNICODE_STRING32_OFFSETLIST.TOTAL_SIZE)
        param_block = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST.CommandLine
        api.WriteMemory(p, bstr)

def set_proc_path(pPEB, pathname):
    if api.IsX64():
        strptr = memrwlib.AllocUNICODE_STRING64(pathname)
        shortname_strptr = memrwlib.AllocUNICODE_STRING64(os.path.basename(pathname))
        bstr = api.ReadMemory(strptr, UNICODE_STRING64_OFFSETLIST.TOTAL_SIZE)
        bstr_shortname = api.ReadMemory(shortname_strptr, UNICODE_STRING64_OFFSETLIST.TOTAL_SIZE)
        param_block = memrwlib.ReadPtr(pPEB+PEB64_OFFSETLIST.ProcessParameters)
        api.WriteMemory(param_block + RTL_USER_PROCESS_PARAMETERS64_OFFSETLIST.ImagePathName, bstr)
        mentry = get_first_module_addr(pPEB)
        api.WriteMemory(mentry + LDR_DATA_TABLE_ENTRY64_OFFSETLIST.FullDllName, bstr)
        api.WriteMemory(mentry + LDR_DATA_TABLE_ENTRY64_OFFSETLIST.BaseDllName, bstr_shortname)
        
    else:
        strptr = memrwlib.AllocUNICODE_STRING32(pathname)
        bstr = api.ReadMemory(strptr, UNICODE_STRING32_OFFSETLIST.TOTAL_SIZE)
        param_block = memrwlib.ReadPtr(pPEB+PEB32_OFFSETLIST.ProcessParameters)
        p = param_block + RTL_USER_PROCESS_PARAMETERS32_OFFSETLIST.ImagePathName
        api.WriteMemory(p, bstr)

allowed_set_names = ['debugger', 'current_dir', 'cmd_line', 'path', 'windows_title', 'global_flags']

def main(args):
    pPEB = memrwlib.NtCurrentPeb()
    if args.action == "set":
        if args.name == None:
            eh.ui.Echo("Inavlid 'name' specified", eh.ECHO_ERROR)
            return
        if args.name not in allowed_set_names:
            eh.ui.Echo("'{}' is not allowed to set".format(args.name), eh.ECHO_ERROR)
            return
        if args.name == "debugger":
            return set_debug_flag(pPEB, int(args.value))
        
        if args.name == "global_flags":
            return set_proc_nt_globalflag(pPEB, int(args.value))

        if args.name == "current_dir":
            return set_current_dir(pPEB, args.value)
        
        if args.name == "cmd_line":
            return set_proc_cmdline(pPEB, args.value)
        
        if args.name == "path":
            return set_proc_path(pPEB, args.value)
        
    elif args.action == "get":
        if args.name == None:
            eh.ui.Echo("Inavlid 'name' specified", eh.ECHO_ERROR)
        if args.name == "all_info":
            return print_all_process_info(pPEB)
        if args.name == "debugger":
            if is_process_debugged(pPEB):
                eh.ui.Echo("TRUE", eh.ECHO_WARNING)
            else:
                eh.ui.Echo("FALSE", eh.ECHO_DEFAULT)

        if args.name == "process_base":
            eh.ui.Echo(hex(get_proc_baseaddress(pPEB)), eh.ECHO_DEFAULT)

        if args.name == "heap_base":
            eh.ui.Echo(hex(get_proc_base_heap(pPEB)), eh.ECHO_DEFAULT)

        if args.name == "subsystem":
            eh.ui.Echo(get_proc_subsystem(pPEB), eh.ECHO_DEFAULT)

        if args.name == "kct":
            eh.ui.Echo(hex(get_proc_kct(pPEB)), eh.ECHO_DEFAULT)

        if args.name == "current_dir":
            eh.ui.Echo(get_proc_curdir(pPEB), eh.ECHO_DEFAULT)

        if args.name == "cmd_line":
            eh.ui.Echo(get_proc_cmdline(pPEB), eh.ECHO_DEFAULT)

        if args.name == "path":
            eh.ui.Echo(get_proc_pathname(pPEB), eh.ECHO_DEFAULT)

        if args.name == "global_flag":
            eh.ui.Echo(hex(get_proc_nt_globalflag(pPEB)), eh.ECHO_DEFAULT)

        if args.name == "window_title":
            eh.ui.Echo(get_wintitle(pPEB), eh.ECHO_DEFAULT)

        if args.name == "module_list":
            eh.ui.Echo(get_loaded_module_list(pPEB), eh.ECHO_DEFAULT)


    else:
        eh.ui.Echo("Inavlid 'action' specified: "+args.action, eh.ECHO_ERROR)



