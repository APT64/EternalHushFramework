import _eternalhush

def Dialog(_str, type=_eternalhush.ctxObj.STRING, default=None):
    """Requests arbitrary user input in the EternalHush console"""
    if isinstance(default, int):
        default = str(default)
    resp = _eternalhush.ctxObj.dialog(_eternalhush.ctxObj.console_id, _str, type, default)
    if type == _eternalhush.ctxObj.INT:
        resp = int(resp)
    return resp


def Option(_str, default=_eternalhush.ctxObj.OPTION_YES):
    """Requests confirmation of the operation in the EternalHush console"""
    return _eternalhush.ctxObj.option(_eternalhush.ctxObj.console_id, _str, default)


def Run(_str, _flag):
    """Executes the specified command in the EternalHsuh console"""
    return _eternalhush.ctxObj.run(_eternalhush.ctxObj.console_id, _str, _flag)


def Echo(_str, _type):
    """Outputs the specified text to the EternalHush console"""
    return _eternalhush.ctxObj.echo(_eternalhush.ctxObj.console_id, _str, _type)
    
    
def SetHostname(_str):
    """Changes the hostname for the current session"""
    return _eternalhush.ctxObj.set_hostname(_eternalhush.ctxObj.console_id, _str)