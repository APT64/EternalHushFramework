import eternalhush as eh
from additional import clingyspider, error_codes
import os

def get_error_name(dict, err_code):
    return dict.keys()[dict.values().index(err_code)]

def ProcessMessage(data):
    err_table = error_codes.error_table
    status = eh.data.Struct(clingyspider.ERROR_STATUS)
    status.from_bytes(data)
    
    status.error_magic = clingyspider.ERROR_MAGIC
    status.status_code = err_table['ERROR_FILE_NOT_FOUND']

    if status.error_magic == clingyspider.ERROR_MAGIC and status.status_code != err_table['STATUS_SUCCESS']:
        err_name = get_error_name(int.from_bytes(status.status_code, 'big'))
        eh.ui.Echo("Implant returned error 0x{0} ({1})".format(status.status_code.hex(), err_name), eh.ECHO_ERROR)
    
    return data