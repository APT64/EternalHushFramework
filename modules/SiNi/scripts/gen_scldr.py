import eternalhush as eh
import datetime
from additional import clingyspider, silentnight
import os

def main(args):
    try:
        with open(args.dll, "rb") as f:
            dll_buf = f.read()
            eh.ui.Echo("Readed "+str(len(dll_buf))+" bytes", eh.ECHO_GOOD)
    except Exception as e:
        eh.ui.Echo(str(e), eh.ECHO_ERROR)
        return
        
    if args.arch.lower() != "win32" and args.arch.lower() != "win64":
        eh.ui.Echo("Invalid arch '"+str(args.arch)+"' specified", eh.ECHO_ERROR)
        return
        
    ldr = silentnight.ReflectiveLoader()
    ldr.use_arch(args.arch)
    if not args.export:
        ldr.set_export_name("RemoteCall")
    else:
        ldr.set_export_name(args.export)
    if not args.exitfunc:
        ldr.set_exitfunc("thread")
    else:
        ldr.set_exitfunc(args.exitfunc)
    ldr.set_nocleanup(args.nocleanup)
    ldr.add_dll(dll_buf)
    sc_buf = ldr.build()
    
    
    out_path = args.out
    if not out_path:
        eh.ui.Echo("Output path not specified, using default path", eh.ECHO_WARNING)
        dt = datetime.datetime.now()
        current_time = dt.strftime("%Y_%m_%d_%Hh%Mm%Ss")
        out_path = os.path.join(eh.ui.GetGlobalEnv("OPERATION_PATH"), "storage\\refldr"+args.arch[3:]+"_"+current_time+".bin")
    try:
        with open(out_path, "wb+") as f:
            f.write(sc_buf)
            eh.ui.Echo("Shellcode stored in "+ out_path, eh.ECHO_GOOD)
            eh.ui.Echo(str(len(sc_buf))+" bytes written", eh.ECHO_GOOD)
    except Exception as e:
        eh.ui.Echo(str(e), eh.ECHO_ERROR)
        return
        