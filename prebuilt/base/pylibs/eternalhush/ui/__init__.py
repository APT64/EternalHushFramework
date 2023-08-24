import _eternalhush

def Dialog(_str):
    """Requests arbitrary user input in the EternalHush console"""
    return _eternalhush.ctxObj.dialog(_str)


def Option(_str):
    """Requests confirmation of the operation in the EternalHush console"""
    return _eternalhush.ctxObj.option(_str)


def Run(_str, _flag):
    """Executes the specified command in the EternalHsuh console"""
    return _eternalhush.ctxObj.run(_str, _flag)


def Echo(_str, _type):
    """Outputs the specified text to the EternalHush console"""
    return _eternalhush.ctxObj.echo(_str, _type)