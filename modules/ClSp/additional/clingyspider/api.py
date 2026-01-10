import eternalhush as eh
from eternalhush.data import _InOut_, _InOutStruct_
from additional.clingyspider import __cmd_builder as cmd_builder
from additional.clingyspider import __response_parser as response_parser
from additional.clingyspider import __tasking as tasking
from additional.clingyspider import __systemapi as systemapi
from eternalhush.exception import DeprecatedApiCall, UnstableApiCall
from additional.clingyspider.warnings import ClingySpider_API
from additional.clingyspider.structs import *
from additional.clingyspider.const import *

def IsX64():
    if eh.ui.GetEnv("MIBA_ARCH") == "X64":
        return 1
    return 0
    
@ClingySpider_API
def GetEnvVar(var_name):
    parser = response_parser.ResponseParser()
    data = tasking.call_one_arg(10, var_name)
    if data:
        parser.load_data(data)
        return parser.get_strarg()

@ClingySpider_API
def CreateFileEx(path, mode, disposition, attributes, options):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(33)
    if not path.startswith('\\'):
        path = '\\??\\' + path
    builder.add_bstrarg(path.encode('utf-16-le'))
    builder.add_int(mode)
    builder.add_int(disposition)
    builder.add_int(attributes)
    builder.add_int(options)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()

@ClingySpider_API
def CreateFile(path, mode, disposition, attributes):
    return CreateFileEx(path, mode, disposition, attributes, 0x00000040)
    
@ClingySpider_API
def CreateDirectory(path, mode, disposition, attributes):
    return CreateFileEx(path, mode, disposition, attributes, 0x00000001)

@ClingySpider_API
def ReadFile(handle):
    parser = response_parser.ResponseParser()
    data = tasking.call_one_iarg(21, handle)
    if data:
        parser.load_data(data)
        size = parser.get_long()
        return parser.get_bstr(size)
    
@ClingySpider_API
def CloseFile(handle):
    parser = response_parser.ResponseParser()
    data = tasking.call_one_iarg(65, handle)
    if data:
        parser.load_data(data)
        return parser.get_int()
    
@ClingySpider_API
def CloseHandle(handle):
    return CloseFile(handle)
    
@ClingySpider_API
def WriteFile(handle, buffer):
    if len(buffer) > 0xffffffffffffffff:
        eh.ui.Echo("Buffer size too big! (MAX 18446744073709551615 bytes)")
        return
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(87)
    builder.add_int(handle)
    builder.add_long(len(buffer))
    builder.add_bstr(buffer)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()
    
@ClingySpider_API
def CreateProcess(proc_name, proc_arg, flags):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(99)
    builder.add_bstrarg(proc_name.encode())
    builder.add_strarg(proc_arg)
    builder.add_int(flags)
    b = builder.build()
    data = tasking.call(b)
    if data:
        proc_info = eh.data.Struct(PROCESS_INFO)
        parser.load_data(data)
        proc_info.pid = parser.get_int()
        proc_info.tid = parser.get_int()
        proc_info.hprocess = parser.get_int()
        proc_info.hthread = parser.get_int()
        proc_info.hread = parser.get_int()
        proc_info.hwrite = parser.get_int()
        return proc_info
    
@ClingySpider_API
def BlockDlls(value):
    parser = response_parser.ResponseParser()
    data = tasking.call_one_iarg(135, value)
    if data:
        parser.load_data(data)
        return parser.get_byte()
        
@ClingySpider_API
def GetProcessToken(handle, access):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(79)
    builder.add_int(handle)
    builder.add_int(access)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()
    
@ClingySpider_API
def GetProcessHandle(pid, access):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(201)
    builder.add_int(pid)
    builder.add_int(access)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()

def GetCurrentThreadToken():
    return -5
    
def GetCurrentProcessToken():
    return -4
    
def GetCurrentProcess():
    return -1

def GetCurrentThread():
    return -2

@ClingySpider_API
def TokenGetUID(handle):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(123)
    builder.add_int(handle)
    builder.add_int(1)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_strarg()
    
@ClingySpider_API
def TokenGetSID(handle):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(123)
    builder.add_int(handle)
    builder.add_int(0)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_strarg()
        
@ClingySpider_API
def VirtualAllocEx(handle, size, alloctype, protection):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(210)
    builder.add_int(handle)
    builder.add_long(size)
    builder.add_int(alloctype)
    builder.add_int(protection)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()
    
def VirtualAlloc(size, alloctype, protection):
    return VirtualAllocEx(GetCurrentProcess(), size, alloctype, protection)

@ClingySpider_API
def InternalVirtualFreeEx(handle, addr, size, freetype):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(211)
    builder.add_int(handle)
    builder.add_long(addr)
    builder.add_long(size)
    builder.add_int(freetype)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()
        
def VirtualFree(addr):
    return InternalVirtualFreeEx(GetCurrentProcess(), addr, 0, MEM_RELEASE)
    
def VirtualDecommit(addr, size):
    return InternalVirtualFreeEx(GetCurrentProcess(), addr, size, MEM_DECOMMIT)
    
def VirtualFreeEx(handle, addr):
    return InternalVirtualFreeEx(handle, addr, 0, MEM_RELEASE)
    
def VirtualDecommitEx(handle, addr, size):
    return InternalVirtualFreeEx(handle, addr, size, MEM_DECOMMIT)

@ClingySpider_API
def VirtualProtectEx(handle, addr, size, protection):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(212)
    builder.add_int(handle)
    builder.add_long(addr)
    builder.add_long(size)
    builder.add_int(protection)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()
        
def VirtualProtect(addr, size, protection):
    return VirtualProtectEx(GetCurrentProcess(), addr, size, protection)

@ClingySpider_API
def WriteMemoryEx(handle, addr, data):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(189)
    builder.add_int(handle)
    builder.add_long(addr)
    builder.add_long(len(data))
    builder.add_bstr(data)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()
    
def WriteMemory(addr, data):
    return WriteMemoryEx(GetCurrentProcess(), addr, data)

@ClingySpider_API
def ReadMemoryEx(handle, addr, size):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(190)
    builder.add_int(handle)
    builder.add_long(addr)
    builder.add_long(size)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        size = parser.get_long()
        return parser.get_bstr(size)
    
def ReadMemory(addr, size):
    return ReadMemoryEx(GetCurrentProcess(), addr, size)

@ClingySpider_API
def ProcessStartApcRoutine(handle, lpstart, lpparam):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(172)
    builder.add_int(handle)
    builder.add_long(lpstart)
    builder.add_long(lpparam)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()
    
@ClingySpider_API
def SectionAlloc(handle, size, sec_protection, protection, sec_attr):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(176)
    builder.add_int(handle)
    builder.add_long(size)
    builder.add_int(sec_protection)
    builder.add_int(protection)
    builder.add_int(sec_attr)
    data = tasking.call(builder.build())
    if data:
        section_info = eh.data.Struct(SECTION_INFO)
        parser.load_data(data)
        section_info.local_ptr = parser.get_long()
        section_info.remote_ptr = parser.get_long()
        return section_info

@ClingySpider_API
def CreateThread(handle, lpstart, lpparam, flags):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(224)
    builder.add_int(handle)
    builder.add_long(lpstart)
    builder.add_long(lpparam)
    builder.add_int(flags)
    data = tasking.call(builder.build())
    if data:
        thread_info = eh.data.Struct(THREAD_INFO)
        parser.load_data(data)
        thread_info.hthread = parser.get_int()
        thread_info.tid = parser.get_int()
        return thread_info

@ClingySpider_API(singlethread=True)
#@UnstableApiCall
def WmiInitialize():
    parser = response_parser.ResponseParser()
    data = tasking.call_no_arg(228, workerctx=systemapi.MBSysGetWorkerContext())
    if data:
        parser.load_data(data)
        return parser.get_byte()

@ClingySpider_API(singlethread=True)
#@UnstableApiCall
def WmiConnect(session_id, srv, user='', passwd='', authority=''):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(229)
    builder.add_int(session_id)
    builder.add_wstrarg(srv)
    builder.add_wstrarg(user)
    builder.add_wstrarg(passwd)
    builder.add_wstrarg(authority)
    data = tasking.call(builder.build(), workerctx=systemapi.MBSysGetWorkerContext())
    if data:
        parser.load_data(data)
        return parser.get_byte()
    
@ClingySpider_API(singlethread=True)
#@UnstableApiCall
def WmiQuery(session_id, query):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(230)
    builder.add_int(session_id)
    builder.add_wstrarg(query)
    data = tasking.call(builder.build(), workerctx=systemapi.MBSysGetWorkerContext())
    if data:
        parser.load_data(data)
        return parser.get_byte()
    
@ClingySpider_API(singlethread=True)
#@UnstableApiCall
def WmiExecMethod(session_id, wmi_class, wmi_method):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(234)
    builder.add_int(session_id)
    builder.add_wstrarg(wmi_class)
    builder.add_wstrarg(wmi_method)
    data = tasking.call(builder.build(), workerctx=systemapi.MBSysGetWorkerContext())
    if data:
        parser.load_data(data)
        return parser.get_byte()
    
@ClingySpider_API(singlethread=True)
#@UnstableApiCall
def WmiParseMethodResult(session_id, filter):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(233)
    builder.add_int(session_id)
    
    builder.add_int(len(filter))
    for i in range(len(filter)):
        builder.add_wstrarg(filter[i])

    data = tasking.call(builder.build(), workerctx=systemapi.MBSysGetWorkerContext())
    if data:
        format_data = []
        parser.load_data(data)
        row_cnt = parser.get_int()
        parser.get_int()
        for i in range(row_cnt):
            unpacked_row = {}
            for label in filter:
                vt_type = parser.get_short()
                if vt_type == 8 or vt_type == 31:
                    unpacked_row[label] = parser.get_wstrarg()
                elif vt_type == 30:
                    unpacked_row[label] = parser.get_strarg()
                elif vt_type == 0 or vt_type == 1 or vt_type == 10:
                    unpacked_row[label] ="(null)"
                elif vt_type == 22 or vt_type == 23 or vt_type == 25 or vt_type == 26 or vt_type == 37 or vt_type == 38 or vt_type == 14 or vt_type == 17 or vt_type == 18 or vt_type == 19 or vt_type == 21 or vt_type == 16 or vt_type == 2 or vt_type == 3 or vt_type == 20 or vt_type == 4 or vt_type == 5 or vt_type == 10:
                    unpacked_row[label] = parser.get_long()
                elif vt_type == 11:
                    unpacked_row[label] = parser.get_byte()
                else:
                    unpacked_row[label] = None
            format_data.append(unpacked_row)
        return format_data
    
@ClingySpider_API(singlethread=True)
#@UnstableApiCall
def WmiParseResult(session_id, filter):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(232)
    builder.add_int(session_id)
    
    builder.add_int(len(filter))
    for i in range(len(filter)):
        builder.add_wstrarg(filter[i])

    data = tasking.call(builder.build(), workerctx=systemapi.MBSysGetWorkerContext())
    if data:
        format_data = []
        parser.load_data(data)
        row_cnt = parser.get_int()
        parser.get_int()
        for i in range(row_cnt):
            unpacked_row = {}
            for label in filter:
                vt_type = parser.get_short()
                if vt_type == 8 or vt_type == 31:
                    unpacked_row[label] = parser.get_wstrarg()
                elif vt_type == 30:
                    unpacked_row[label] = parser.get_strarg()
                elif vt_type == 0 or vt_type == 1:
                    unpacked_row[label] ="<empty>"
                elif vt_type == 22 or vt_type == 23 or vt_type == 25 or vt_type == 26 or vt_type == 37 or vt_type == 38 or vt_type == 14 or vt_type == 17 or vt_type == 18 or vt_type == 19 or vt_type == 21 or vt_type == 16 or vt_type == 2 or vt_type == 3 or vt_type == 20 or vt_type == 4 or vt_type == 5 or vt_type == 10:
                    unpacked_row[label] = parser.get_long()
                elif vt_type == 11:
                    unpacked_row[label] = parser.get_byte()
                else:
                    unpacked_row[label] = None
            format_data.append(unpacked_row)
        return format_data
    
@ClingySpider_API(singlethread=True)
#@UnstableApiCall
def WmiRelease(session_id):
    parser = response_parser.ResponseParser()
    data = tasking.call_one_iarg(231, session_id, workerctx=systemapi.MBSysGetWorkerContext())
    if data:
        parser.load_data(data)
        return parser.get_byte()
    
@ClingySpider_API
def GetVirtualWorkdir():
    parser = response_parser.ResponseParser()
    data = tasking.call_no_arg(163)
    if data:
        parser.load_data(data)
        return parser.get_wstrarg()
    
@ClingySpider_API
def SetVirtualWorkdir(path):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(164)
    wpath = path.encode("utf-16-le")
    wpath += b"\x00" * eh.data.RoundToDiv(len(wpath), 16)
    builder.add_bstrarg(wpath)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()
    
@ClingySpider_API
def MemLoadLibraryInternal(ptr, size, flag_noexport, flag_nowait, byordinal, ordinal, export_name):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(98)
    builder.add_byte(flag_nowait)
    builder.add_long(ptr)
    builder.add_int(size)
    builder.add_byte(flag_noexport)
    builder.add_byte(flag_nowait)
    builder.add_byte(byordinal)
    if not flag_noexport:
        if byordinal:
            builder.add_int(ordinal)
        else:
            builder.add_int(eh.crypto.PJWHASH(export_name.encode('utf-8')))
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()
    
def MemLoadLibrary(ptr, size, export, nowait):
    return MemLoadLibraryInternal(ptr, size, 0, nowait, 0, 0, export)

def MemLoadLibraryNoExport(ptr, size, nowait):
    return MemLoadLibraryInternal(ptr, size, 1, nowait, 0, 0, 0)

def MemLoadLibraryOrdinal(ptr, size, ordinal, nowait):
    return MemLoadLibraryInternal(ptr, size, 0, nowait, 1, ordinal, 0)

@ClingySpider_API
def MemMapLibraryFromBuffer(ptr, size):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(95)
    builder.add_long(ptr)
    builder.add_int(size)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()
    
@ClingySpider_API
def MemUnmapLibraryFromBuffer(ptr):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(96)
    builder.add_long(ptr)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()

@ClingySpider_API
def MemGetProcAddress(ptr, export_name):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(97)
    builder.add_long(ptr)
    builder.add_int(eh.crypto.PJWHASH(export_name.encode()))
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()

@ClingySpider_API
def MemGetProcAddressEx(hprocess, ptr, export_name):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(34)
    builder.add_int(hprocess)
    builder.add_long(ptr)
    builder.add_int(eh.crypto.PJWHASH(export_name.encode()))
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()
        
def MemGetModuleHandleEx(hprocess, export_name):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(35)
    builder.add_int(hprocess)
    builder.add_int(eh.crypto.PJWHASH(export_name.encode()))
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()

@ClingySpider_API
def MemGetModuleInfoInternal(export_name, action_type):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(100)
    builder.add_int(eh.crypto.PJWHASH(export_name.encode('utf-16-le')))
    builder.add_int(action_type)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()


def MemGetModuleHandle(export_name):
    return MemGetModuleInfoInternal(export_name, 0)

def MemGetModuleSize(export_name):
    return MemGetModuleInfoInternal(export_name, 1)

@ClingySpider_API
def RegOpenKey(hkey, subkey, access):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(249)
    builder.add_int(hkey)
    builder.add_wstrarg(subkey)
    builder.add_int(access)
        
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()

@ClingySpider_API
def RegCreateKey(hkey, subkey, access):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(250)
    builder.add_int(hkey)
    builder.add_wstrarg(subkey)
    builder.add_int(access)
        
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()

@ClingySpider_API
def RegQueryValue(hkey, subkey):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(251)
    builder.add_int(hkey)
    builder.add_wstrarg(subkey)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        reg_type = parser.get_int()
        reg_size = parser.get_int()
        reg_data = parser.get_bstrarg()

        RESIZED_REG_VALUE_INFO = REG_VALUE_INFO
        RESIZED_REG_VALUE_INFO['data'] = reg_size
        ret_info = eh.data.Struct(RESIZED_REG_VALUE_INFO)

        ret_info.type = reg_type
        ret_info.size = reg_size
        ret_info.data = reg_data
        return ret_info

@ClingySpider_API
def RegEnumInfoInternal(hkey, action_type):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(252)
    builder.add_int(hkey)
    builder.add_byte(action_type)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        entry_count = parser.get_int()
        entry_list = []
        for i in range(entry_count):
            entry_list.append(parser.get_wstrarg())
        return entry_list

def RegEnumKey(hkey):
    return RegEnumInfoInternal(hkey, 0x1)

def RegEnumValue(hkey):
    return RegEnumInfoInternal(hkey, 0x0)

@ClingySpider_API
def RegCloseKey(hkey):
    parser = response_parser.ResponseParser()
    data = tasking.call_one_iarg(253, hkey)
    if data:
        parser.load_data(data)
        return parser.get_int()

@ClingySpider_API
def RegSetValue(hkey, value_name, value_type, value_data):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(248)
    builder.add_int(hkey)
    builder.add_wstrarg(value_name)
    builder.add_int(value_type)
    if value_type == REG_SZ or value_type == REG_EXPAND_SZ:
        builder.add_wstrarg(value_data)
    elif value_type == REG_MULTI_SZ:
        data = b""
        for e in value_data:
            data += e.encode("utf-16-le") + b"\x00\x00"
        data+=b"\x00\x00"
        builder.add_bstrarg(data)
    elif value_type == REG_DWORD or value_type == REG_DWORD_LITTLE_ENDIAN:
        data = int.to_bytes(value_data, 4, 'little')
        builder.add_bstrarg(data)
    elif value_type == REG_QWORD or value_type == REG_QWORD_LITTLE_ENDIAN:
        data = int.to_bytes(value_data, 8, 'little')
        builder.add_bstrarg(data)
    elif value_type == REG_DWORD_BIG_ENDIAN:
        data = int.to_bytes(value_data, 4, 'big')
        builder.add_bstrarg(data)
    elif value_type == REG_BINARY:
        builder.add_bstrarg(value_data)
    else:
        eh.ui.Echo("RegSetValue: invalid data type specified!", eh.ECHO_ERROR)
        return None

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()

@ClingySpider_API
def RegDeleteInfoInternal(hkey, value_name, delete_type):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(247)
    builder.add_int(hkey)
    builder.add_wstrarg(value_name)
    builder.add_int(delete_type)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()
    
def RegDeleteKey(hkey, name):
    return RegDeleteInfoInternal(hkey, name, 0x1)

def RegDeleteValue(hkey, name):
    return RegDeleteInfoInternal(hkey, name, 0x0)

@ClingySpider_API
def ReadGeneralSegment(offset, size):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(77)
    builder.add_int(offset)
    builder.add_int(size)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()
    
@ClingySpider_API
def TerminateThread(hthread, exit_code):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(103)
    builder.add_int(hthread)
    builder.add_int(exit_code)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()
    
@ClingySpider_API
def TerminateProcess(hprocess, exit_code):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(102)
    builder.add_int(hprocess)
    builder.add_int(exit_code)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()
 
@ClingySpider_API
def LocalAlloc(size):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(245)
    builder.add_long(size)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()
    
@ClingySpider_API
def LocalFree(ptr):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(246)
    builder.add_long(ptr)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()

@ClingySpider_API
def UserSyscall(syscall_name, *varargs):
    varargs = tuple(reversed(varargs))
    builder = cmd_builder.CommandBuilder()
    builder.set_command(244)

    syscall_hash = eh.crypto.PJWHASH(syscall_name.encode())
    builder.add_int(syscall_hash)
    builder.add_int(len(varargs))

    is_64 = IsX64()
    for arg in varargs:
        if is_64:
            builder.add_long(arg)
        else:
            builder.add_int(arg)
    
    tasking.call(builder.build())
    return eh.ui.GetLastError()    

def _pack_usercall_args(builder, arg, retn):
    ATypeLong =      3
    ATypeRawMemory = 4
    builder.add_byte(retn)
    if isinstance(arg, int):
        builder.add_byte(ATypeLong)
        builder.add_long(arg)
    if isinstance(arg, bytes) or isinstance(arg, bytearray):
        builder.add_byte(ATypeRawMemory)
        builder.add_bstrarg(arg)
    if isinstance(arg, eh.data.Struct):
        builder.add_byte(ATypeRawMemory)
        builder.add_bstrarg(arg.data())
    if isinstance(arg, str):
        builder.add_byte(ATypeRawMemory)
        builder.add_wstrarg(arg)

@ClingySpider_API
@DeprecatedApiCall
def UserSyscall2(syscall_name, *varargs):
    ret_args_list = []
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(244)

    syscall_hash = eh.crypto.PJWHASH(syscall_name)
    builder.add_int(syscall_hash)
    builder.add_int(len(varargs))

    for arg in varargs:
        if isinstance(arg, _InOut_):
            ret_args_list.append(arg)
            _pack_usercall_args(builder, arg.get(), 1)
        elif isinstance(arg, _InOutStruct_):
            ret_args_list.append(arg)
            _pack_usercall_args(builder, arg.get().data(), 1)
        else:
            _pack_usercall_args(builder, arg, 0)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        ret_argc = parser.get_int()
        for i in range(ret_argc):
            if isinstance(ret_args_list[i], _InOut_):
                if ret_args_list[i].is_int():
                    ret_args_list[i].set(parser.get_long())
                elif ret_args_list[i].is_bytes():
                    ret_args_list[i].set(parser.get_bstrarg())
                elif ret_args_list[i].is_str():
                    ret_args_list[i].set(parser.get_wstrarg())
            if isinstance(ret_args_list[i], _InOutStruct_):
                ret_args_list[i].set(parser.get_bstrarg())
    return eh.ui.GetLastError()

@ClingySpider_API
def CaptureWindowImage(hwnd, quality):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(155)
    builder.add_long(hwnd)
    builder.add_int(quality)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        size = parser.get_long()
        return parser.get_bstr(size)
    
@ClingySpider_API
def GetWindowByPid(pid):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(156)
    builder.add_int(pid)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()

@ClingySpider_API
def AdjustPrivilegesToken(htoken, privname, state):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(137)
    builder.add_int(htoken)
    builder.add_int(state)
    builder.add_strarg(privname)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_int()
    
@ClingySpider_API
def QueryPrivilegesToken(htoken, privs):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(138)
    builder.add_int(htoken)
    builder.add_int(len(privs))
    for s in privs:
        builder.add_strarg(s)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return_set = {}
        count = parser.get_int()
        for i in range(count):
            b = parser.get_byte()
            return_set[privs[i]] = bool(b)
        return return_set

@ClingySpider_API
def GetNetworkParameters():
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(59)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        params = eh.data.Struct(NETWORK_PARAMS)
        params.hostname = parser.get_strarg()
        params.domainname = parser.get_strarg()
        dns_count = parser.get_int()
        params.dns_count = dns_count
        dns_buffer = b""
        for i in range(dns_count):
            dns_server = parser.get_strarg()
            ip_addr = eh.data.Struct(STR_IP_ADDR)
            ip_addr.addr = dns_server
            dns_buffer += ip_addr.data()
        params.dns_buffer = dns_buffer
        params.node_type = parser.get_int()
        params.enable_routing = parser.get_int()
        params.enable_proxy = parser.get_int()
        params.enable_dns = parser.get_int()
        return params

@ClingySpider_API
def GetNetworkAdapters():
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(148)
    data = tasking.call(builder.build())
    if data:
        adapters_list = []
        parser.load_data(data)
        adapters_count = parser.get_int()
        for i in range(adapters_count):
            netadapter_info = eh.data.Struct(NETWORK_ADAPTER_INFO)
            netadapter_info.type = parser.get_int()
            netadapter_info.name = parser.get_strarg()
            netadapter_info.description = parser.get_strarg()
            netadapter_info.physical_address = parser.get_strarg()
            netadapter_info.enable_dhcp = parser.get_int()
            netaddr_count = parser.get_int()
            netadapter_info.netaddr_count = netaddr_count
            netinfo_buffer = b""
            for i in range(netaddr_count):
                ip_address = parser.get_strarg()
                netmask_address = parser.get_strarg()
                ip_addr = eh.data.Struct(STR_IP_ADDR)
                ip_addr.addr = ip_address
                netinfo_buffer += ip_addr.data()
                ip_addr.addr = netmask_address
                netinfo_buffer += ip_addr.data()
            
            netadapter_info.netaddr_buffer = netinfo_buffer
            gateway_count = parser.get_int()
            netadapter_info.gateway_count = gateway_count
            gateway_buffer = b""

            for i in range(gateway_count):
                gateway_address = parser.get_strarg()
                ip_addr = eh.data.Struct(STR_IP_ADDR)
                ip_addr.addr = gateway_address
                gateway_buffer += ip_addr.data()

            netadapter_info.gateway_buffer = gateway_buffer
            netadapter_info.dhcp_server = parser.get_strarg()
            adapters_list.append(netadapter_info)

        return adapters_list


@ClingySpider_API
def QueryNetworkRoutes(inet_family):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(68)

    if inet_family != AF_INET:
        eh.ui.Echo("QueryNetworkRoutes: Currently only AF_INET is supported", eh.ECHO_ERROR)
        return

    builder.add_int(inet_family)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        routes_array = []
        route_count = parser.get_int()
        for i in range(route_count):
            route_info = eh.data.Struct(NETWORK_ROUTE_INFO)
            route_info.destination_address = parser.get_strarg()
            route_info.netmask = parser.get_strarg()
            route_info.gateway = parser.get_strarg()
            route_info.interface_address = parser.get_strarg()
            route_info.interface_id = parser.get_int()
            route_info.metric = parser.get_int()
            route_info.origin = parser.get_int()
            routes_array.append(route_info)
        return routes_array

@ClingySpider_API
def InternalModifyNetworkRoutes(action, inet_family, dest_addr, gateway_address, netmask, iface_idx, metric):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(81)

    if inet_family != AF_INET:
        eh.ui.Echo("InternalModifyNetworkRoutes: Currently only AF_INET is supported", eh.ECHO_ERROR)
        return

    builder.add_int(action)
    builder.add_int(inet_family)
    builder.add_strarg(dest_addr)
    builder.add_strarg(gateway_address)
    builder.add_strarg(netmask)
    builder.add_int(iface_idx)
    builder.add_int(metric)

    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()

@ClingySpider_API
def AddNetworkRoute(inet_family, dest_addr, gateway_address, netmask, iface_idx, metric):
    return InternalModifyNetworkRoutes(0, inet_family, dest_addr, gateway_address, netmask, iface_idx, metric)

@ClingySpider_API
def DeleteNetworkRoute(inet_family, dest_addr, gateway_address, netmask, iface_idx):
    return InternalModifyNetworkRoutes(1, inet_family, dest_addr, gateway_address, netmask, iface_idx, 0)