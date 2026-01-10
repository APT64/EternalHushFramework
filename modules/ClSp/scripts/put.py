import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const

def main(args):
    source_file = args.src
    destination_file = args.out

    try:
        with open(source_file, "rb") as f:
            content = f.read()
            content = bytearray(content)
    except IOError:
        eh.ui.Echo("Error while opening the file", eh.ECHO_ERROR)
        return  
        
    handle = api.CreateFile(destination_file, const.GENERIC_WRITE, const.FILE_CREATE, const.FILE_ATTRIBUTE_NORMAL)
    if not handle:
        eh.ui.Echo("Failed to open remote file", eh.ECHO_ERROR)
        return
    bytes_len = api.WriteFile(handle, content)
    if bytes_len == None:
        eh.ui.Echo("Failed to write remote file", eh.ECHO_ERROR)
        return
    if not api.CloseFile(handle):
        eh.ui.Echo("Failed to close remote handle", eh.ECHO_WARNING)
    eh.ui.Echo("Written {} bytes".format(bytes_len), eh.ECHO_GOOD)
        


