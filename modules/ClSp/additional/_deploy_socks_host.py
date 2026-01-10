import eternalhush as eh
from additional import clingyspider
import os

CONFIG_MAGIC = 0xcafebabecafebabe
HOST_CONFIG = {
    "magic_value": eh.LONGLONG,
    "rport" : eh.LONG,
    "lport" : eh.LONG
}

def configure_hostdll(args, outpath):
    if args.arch.lower() == "win64":
        dll_name = "X64_proxyhost.dll"
    elif args.arch.lower() == "win32":
        dll_name = "X32_proxyhost.dll"
    else:
        eh.ui.Echo("Unknown arch!", eh.ECHO_ERROR)
        return
    try:
        with open(os.path.join(eh.FILEDIR, dll_name), "rb") as f:
            dll_buf = f.read()
    except Exception as e:
        eh.ui.Echo(str(e), eh.ECHO_ERROR)
        return

    magic_val = dll_buf.find(CONFIG_MAGIC.to_bytes(eh.LONGLONG, "little"))
    if magic_val > 0:
        config = eh.data.Struct(HOST_CONFIG)
        config.from_bytes(dll_buf, global_offset=magic_val)
        
        config.rport = args.port
        config.lport = args.local_port
        out_path = os.path.join(outpath, "proxyhost"+dll_name[1:3]+".dll")
        try:
            with open(out_path, "wb+") as f:
                wr_bytes = f.write(config.full_data())
                eh.ui.Echo("Configured proxyhost dll at {}".format(out_path), eh.ECHO_GOOD)
                if args.shellcode:
                    eh.ui.Echo("Generating reflective shellcode...", eh.ECHO_DEFAULT)
                    eh.ui.Run("!gen_scldr -dll {} -arch {} -out {}".format(out_path, "win"+dll_name[1:3],os.path.join(outpath, "proxyhost"+dll_name[1:3]+".bin")), eh.RUN_SILENT)
                    eh.ui.Echo("Configured proxyhost shellcode at {}".format(out_path[:-3]+"bin"), eh.ECHO_GOOD)
        except IOError:
            eh.ui.Echo("Error while opening the file "+ out_path, eh.ECHO_ERROR)
            return