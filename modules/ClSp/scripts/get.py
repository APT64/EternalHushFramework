import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const

def main(args):
    source_file = args.src
    dest_file = args.out
    
    handle = api.CreateFile(source_file, const.GENERIC_READ, const.FILE_OPEN, const.FILE_ATTRIBUTE_NORMAL)
    if not handle:
        eh.ui.Echo("Failed to open remote file", eh.ECHO_ERROR)
        return

    bytes = api.ReadFile(handle)
    if bytes == None:
        eh.ui.Echo("Failed to read remote file", eh.ECHO_ERROR)
        return
    try:
        with open(dest_file, "wb+") as f:
            f.write(bytes)

    except IOError:
        eh.ui.Echo("Error while opening the file", eh.ECHO_ERROR)
        return
    
    eh.ui.Echo("Written {} bytes".format(len(bytes)), eh.ECHO_GOOD)
    if not api.CloseFile(handle):
        eh.ui.Echo("Failed to close remote handle", eh.ECHO_WARNING)
    


