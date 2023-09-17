import eternalhush as eh
from additional import clingyspider
import os

USERNAME = {
    "username_length": eh.SHORT,
    "username": 254,
}

def main(args):
    clingyspider.ExecuteModule(os.path.join(FILEDIR, "X64_Whoami.dll"))
    connection = int(eh.ui.GetEnv("CLSP_CONNECTION"))
    session_key = bytearray.fromhex(eh.ui.GetEnv("CLSP_KEY"))
    next_iv = bytearray.fromhex(eh.ui.GetEnv("CLSP_IV"))
    
    encrypted_username = eh.net.TcpRecv(connection, 256)
    
    username = eh.data.Struct(USERNAME)
    username.from_bytes(eh.crypto.DecryptAesData(session_key, next_iv, encrypted_username))
    length = int.from_bytes(username.username_length, "little")
    
    user = username.username[:length].decode("utf-8")
    eh.ui.Echo(user, eh.ECHO_DEFAULT)
    eh.ui.SetEnv("CLSP_USER", user)
    
if __name__ == "__main__":
    main(sys.argv)