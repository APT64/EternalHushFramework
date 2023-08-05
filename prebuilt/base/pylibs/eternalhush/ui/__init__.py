import _eternalhush

def Dialog(_str):
    return _eternalhush.ctxObj.dialog(_str)


def Option(_str):
    return _eternalhush.ctxObj.option(_str)


def Run(_str, _flag):
    return _eternalhush.ctxObj.run(_str, _flag)


def Echo(_str, _type):
    return _eternalhush.ctxObj.echo(_str, _type)