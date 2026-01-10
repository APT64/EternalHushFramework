from eternalhush.data import Struct, Field, offsetof
from additional.clingyspider import api, const
import eternalhush as eh
from additional.clingyspider.warnings import ClingySpider_API
from additional.clingyspider.nt_const import *

@ClingySpider_API
def ReadByte(ptr, handle=None):
    if not handle:
        return int.from_bytes(api.ReadMemory(ptr, eh.CHAR), 'little')
    return int.from_bytes(api.ReadMemoryEx(handle, ptr, eh.CHAR), 'little')
    
@ClingySpider_API
def ReadWord(ptr, handle=None):
    if not handle:
        return int.from_bytes(api.ReadMemory(ptr, eh.SHORT), 'little')
    return int.from_bytes(api.ReadMemoryEx(handle, ptr, eh.SHORT), 'little')
    
@ClingySpider_API
def ReadDword(ptr, handle=None):
    if not handle:
        return int.from_bytes(api.ReadMemory(ptr, eh.LONG), 'little')
    return int.from_bytes(api.ReadMemoryEx(handle, ptr, eh.LONG), 'little')
    
@ClingySpider_API
def ReadLlong(ptr, handle=None):
    if not handle:
        return int.from_bytes(api.ReadMemory(ptr, eh.LONGLONG), 'little')
    return int.from_bytes(api.ReadMemoryEx(handle, ptr, eh.LONGLONG), 'little')
    
@ClingySpider_API
def WriteByte(ptr, value, handle=None):
    if not handle:
        return int(api.WriteMemory(ptr, value.to_bytes(eh.CHAR, 'little')))
    return int(api.WriteMemoryEx(handle, ptr, value.to_bytes(eh.CHAR, 'little')))

@ClingySpider_API 
def WriteWord(ptr, value, handle=None):
    if not handle:
        return int(api.WriteMemory(ptr, value.to_bytes(eh.SHORT, 'little')))
    return int(api.WriteMemoryEx(handle, ptr, value.to_bytes(eh.SHORT, 'little')))
    
@ClingySpider_API
def WriteDword(ptr, value, handle=None):
    if not handle:
        return int(api.WriteMemory(ptr, value.to_bytes(eh.LONG, 'little')))
    return int(api.WriteMemoryEx(handle, ptr, value.to_bytes(eh.LONG, 'little')))
    
@ClingySpider_API
def WriteLlong(ptr, value, handle=None):
    if not handle:
        return int(api.WriteMemory(ptr, value.to_bytes(eh.LONGLONG, 'little')))
    return int(api.WriteMemoryEx(handle, ptr, value.to_bytes(eh.LONGLONG, 'little')))

@ClingySpider_API
def ReadPtr(ptr, handle=None):
    if api.IsX64():
        return ReadLlong(ptr, handle=handle)
    return ReadDword(ptr, handle=handle)

@ClingySpider_API
def AllocByte(handle=None):
    if not handle:
        memptr = api.LocalAlloc(eh.CHAR)
    else:
        memptr = api.VirtualAllocEx(handle, eh.CHAR, const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    return memptr

@ClingySpider_API
def AllocWord(handle=None):
    if not handle:
        memptr = api.LocalAlloc(eh.SHORT)
    else:
        memptr = api.VirtualAllocEx(handle, eh.SHORT, const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    return memptr

@ClingySpider_API
def AllocDword(handle=None):
    if not handle:
        memptr = api.LocalAlloc(eh.LONG)
    else:
        memptr = api.VirtualAllocEx(handle, eh.LONG, const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    return memptr

@ClingySpider_API
def AllocLlong(handle=None):
    if not handle:
        memptr = api.LocalAlloc(eh.LONGLONG)
    else:
        memptr = api.VirtualAllocEx(handle, eh.LONGLONG, const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    return memptr

@ClingySpider_API
def WritePtr(ptr, value, handle=None):
    if api.IsX64():
        return WriteLlong(ptr, value, handle=handle)
    return WriteDword(ptr, value, handle=handle)

@ClingySpider_API
def ReadUnicodeString(ptr, size, handle=None):
    if not handle:
        return api.ReadMemory(ptr, size).decode('utf-16-le')
    return api.ReadMemoryEx(handle, ptr, size).decode('utf-16-le')

@ClingySpider_API
def ReadAnsiString(ptr, size, handle=None):
    if not handle:
        return api.ReadMemory(ptr, size).decode()
    return api.ReadMemoryEx(handle, ptr, size).decode()

@ClingySpider_API
def StructAllocate(definition, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    structure = MemoryAccessStruct(definition, 0, handle=handle)
    size = len(structure)
    if handle == api.GetCurrentProcess():
        memptr = api.LocalAlloc(size)
        structure.allocation_type = "local_heap"
    else:
        memptr = api.VirtualAllocEx(handle, size, const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
        structure.allocation_type = "virtual_mem"
    structure.memptr = memptr

    return structure

@ClingySpider_API
def PtrCastTo(ptr, definition, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    if isinstance(ptr, eh.data.Field):
        ptr = ptr.get(int)
    return MemoryAccessStruct(definition, ptr, handle=handle)

@ClingySpider_API
def ParseUNICODE_STRING32(ptr, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    buffer_ptr = ReadPtr(ptr + UNICODE_STRING32_OFFSETLIST.Buffer, handle=handle)
    str_size = ReadWord(ptr + UNICODE_STRING32_OFFSETLIST.Length, handle=handle)
    return ReadUnicodeString(buffer_ptr, str_size, handle=handle)

@ClingySpider_API
def ParseUNICODE_STRING64(ptr, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    buffer_ptr = ReadPtr(ptr + UNICODE_STRING64_OFFSETLIST.Buffer, handle=handle)
    str_size = ReadWord(ptr + UNICODE_STRING64_OFFSETLIST.Length, handle=handle)
    return ReadUnicodeString(buffer_ptr, str_size, handle=handle)

@ClingySpider_API
def ParseSTRING32(ptr, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    buffer_ptr = ReadPtr(ptr + STRING32_OFFSETLIST.Buffer, handle=handle)
    str_size = ReadWord(ptr + STRING32_OFFSETLIST.Length, handle=handle)
    return ReadAnsiString(buffer_ptr, str_size, handle=handle)

@ClingySpider_API
def ParseSTRING64(ptr, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    buffer_ptr = ReadPtr(ptr + STRING64_OFFSETLIST.Buffer, handle=handle)
    str_size = ReadWord(ptr + STRING64_OFFSETLIST.Length, handle=handle)
    return ReadAnsiString(buffer_ptr, str_size, handle=handle)
    

@ClingySpider_API
def NtCurrentPeb():
    if api.IsX64():
        pPEB = api.ReadGeneralSegment(PEB64_OFFSETLIST.OFFSET_PEB_SEGMENT, eh.LONG)
    else:
        pPEB = api.ReadGeneralSegment(PEB32_OFFSETLIST.OFFSET_PEB_SEGMENT, eh.LONG)
    return pPEB

@ClingySpider_API
def AllocUNICODE_STRING64(string, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    bytestring = string.encode('utf-16-le')
    if handle == api.GetCurrentProcess():
        strptr = api.LocalAlloc(len(bytestring))
        ptr = api.LocalAlloc(UNICODE_STRING64_OFFSETLIST.TOTAL_SIZE)
    else:
        strptr = api.VirtualAllocEx(handle, len(bytestring), const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
        ptr = api.VirtualAllocEx(handle, UNICODE_STRING64_OFFSETLIST.TOTAL_SIZE, const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    
    api.WriteMemoryEx(handle, strptr, bytestring)
    WritePtr(ptr + UNICODE_STRING64_OFFSETLIST.Buffer, strptr, handle=handle)
    WriteWord(ptr + UNICODE_STRING64_OFFSETLIST.Length, len(bytestring), handle=handle)
    WriteWord(ptr + UNICODE_STRING64_OFFSETLIST.Length, len(bytestring), handle=handle)

    return ptr

@ClingySpider_API
def AllocUNICODE_STRING32(string, handle=None):
    if not handle:
        handle = api.GetCurrentProcess()
    bytestring = string.encode('utf-16-le')
    if handle == api.GetCurrentProcess():
        strptr = api.LocalAlloc(len(bytestring))
        ptr = api.LocalAlloc(UNICODE_STRING32_OFFSETLIST.TOTAL_SIZE)
    else:
        strptr = api.VirtualAllocEx(handle, len(bytestring), const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
        ptr = api.VirtualAllocEx(handle, UNICODE_STRING32_OFFSETLIST.TOTAL_SIZE, const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    
    api.WriteMemoryEx(handle, strptr, bytestring)
    WritePtr(ptr + UNICODE_STRING32_OFFSETLIST.Buffer, strptr, handle=handle)
    WriteWord(ptr + UNICODE_STRING32_OFFSETLIST.Length, len(bytestring), handle=handle)
    WriteWord(ptr + UNICODE_STRING32_OFFSETLIST.Length, len(bytestring), handle=handle)

    return ptr

#HIGHLY NOT RECOMMENDED TO USE!
class MemoryAccessStruct(Struct):
    allocation_type = None
    freed = False
    def __init__(self, scheme, memptr, handle=api.GetCurrentProcess()):
        Struct.__init__(self, scheme)
        self.memptr = memptr
        self.handle = handle

    def __setattr__(self, attrname, value):
        if attrname in self._definition.keys():
            self.__dict__[attrname].set(value)
            offset = offsetof(self, attrname)[0]
            if isinstance(value, int):
                value = value.to_bytes(self._definition.get(attrname), byteorder="little")            
            elif isinstance(value, bytes):
                value = value
            elif isinstance(value, bytearray):
                value = value
            elif isinstance(value, str):
                value = bytes(value, 'utf-8')
            api.WriteMemoryEx(self.handle, self.memptr+offset+self.g_offset, value)
        else:
            self.__dict__[attrname] = value
    
    def __getattribute__(self, attrname):
        attr = object.__getattribute__(self, attrname)
        if isinstance(attr, Field):
            offset = offsetof(self, attrname)[0]
            byte_result = api.ReadMemoryEx(self.handle, self.memptr+offset+self.g_offset, self._definition[attrname])
            attr.set(byte_result)
            return self.__dict__[attrname]
        return attr

    def data(self):
        return api.ReadMemoryEx(self.handle, self.memptr, self.struct_size)
    
    def addressof(self, name):
        return self.memptr + offsetof(self, name)[0]

    def ptr(self):
        return self.memptr
    
    def free(self):
        if self.freed:
            raise MemoryError("Can't allow double-free!")
        if self.allocation_type == "local_heap":
            api.LocalFree(self.memptr)
        elif self.allocation_type == "virtual_mem":
            api.VirtualFreeEx(self.handle, self.memptr)
        self.freed = True
    
    def __del__(self):
        if not self.freed:
            self.free()
