import eternalhush as eh
from additional.clingyspider import api, extapi
from additional.clingyspider import const
from additional import memrwlib
from additional.clingyspider.nt_const import *
import additional.darknarrator as dana
import os
import time

def main(args):
    if args.register:
        if eh.ui.GetEnv("DANA_REGISTERED") == 'true':
            eh.ui.Echo(f"Extension already registered!", eh.ECHO_ERROR)
            return

        if api.IsX64():
            ext_name = "X64_DarkNarratorExt.dll"
        else:
            ext_name = "X32_DarkNarratorExt.dll"

        byte_dll = open(os.path.join(eh.FILEDIR, ext_name), "rb").read()
        if not byte_dll:
            eh.ui.Echo("Failed to read extension dll!", eh.ECHO_ERROR)
            return

        mptr = api.LocalAlloc(len(byte_dll))
        written_bytes = api.WriteMemory(mptr, byte_dll)
        eh.ui.Echo(f"Extension dll written to {hex(mptr)} ({written_bytes} bytes)", eh.ECHO_DEFAULT)

        mapped = api.MemMapLibraryFromBuffer(mptr, len(byte_dll))
        eh.ui.Echo(f"Extension mapped to {hex(mapped)}", eh.ECHO_DEFAULT)

        api.LocalFree(mptr)

        res = extapi.RegisterUserExtension(mapped)
        if res:
            eh.ui.Echo(f"Extension successfully registered!", eh.ECHO_GOOD)
            eh.ui.SetEnv("DANA_REGISTERED", 'true')
        else:
            api.MemUnmapLibraryFromBuffer(mapped)
            eh.ui.SetEnv("DANA_REGISTERED", 'false')

    elif args.unregister:
        if eh.ui.GetEnv("DANA_REGISTERED") != 'true':
            eh.ui.Echo("DarkNarrator extension not registered!", eh.ECHO_ERROR)
            return
        res = extapi.EnumUserExtensions()
        if res == None:
            eh.ui.Echo("Failed to enumerate user extensions!", eh.ECHO_ERROR)
            return

        for e in res:
            if e['ext_uid'] == dana.DARKNARRATOR_UID:
                api.MemUnmapLibraryFromBuffer(e['ext_base'])

        res = extapi.UnregisterUserExtension(dana.DARKNARRATOR_UID)
        if res:
            eh.ui.Echo(f"Extension successfully unregistered!", eh.ECHO_GOOD)
            eh.ui.SetEnv("DANA_REGISTERED", 'false')
        else:
            eh.ui.Echo(f"Failed to unregister extension! {hex(api.GetLastError())}", eh.ECHO_ERROR)
    else:
        eh.ui.Echo(f"Specify -register/-unregister option!", eh.ECHO_ERROR)
        return