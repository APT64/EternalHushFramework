import eternalhush as eh
from additional import winerror

def main(args):
    error_code = args.code
    if not error_code[:2] == "0x":
        error_code = "0x"+error_code
    if error_code[:3] == "0xc":
        name = winerror.get_ntstatus(int(error_code, 16))
    else:
        name = winerror.get_winerror(int(error_code, 16))
    if not name:
        name = "UNKNOWN_ERROR_CODE"
    eh.ui.Echo(name, eh.ECHO_DEFAULT)
