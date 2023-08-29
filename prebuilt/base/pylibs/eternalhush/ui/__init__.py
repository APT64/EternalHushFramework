import _eternalhush

def Dialog(_str, type=_eternalhush.ctxObj.STRING, default=None):
    """Requests arbitrary user input in the EternalHush console"""
    if isinstance(default, int):
        default = str(default)
        
    resp = _eternalhush.ctxObj.dialog(_str, type, default)
    if type == _eternalhush.ctxObj.INT:
        resp = int(resp)
    return resp


def Option(_str, default=_eternalhush.ctxObj.OPTION_YES):
    """Requests confirmation of the operation in the EternalHush console"""
    return _eternalhush.ctxObj.option(_str, default)


def Run(_str, _flag):
    """Executes the specified command in the EternalHsuh console"""
    return _eternalhush.ctxObj.run(_str, _flag)


def Echo(_str, _type):
    """Outputs the specified text to the EternalHush console"""
    return _eternalhush.ctxObj.echo(_str, _type)