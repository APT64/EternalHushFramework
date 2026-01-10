import _eternalhush

def FormatTable(headers, data, max_elem_sz = 20):
    ret_str = ""
    format_string = ""
    
    for i in range(len(headers)):
        max_element_len = max_elem_sz
        for entry in data:
            elem = entry[i]
            if len(str(elem)) > max_element_len:
                max_element_len = len(str(elem))
        format_string += "{:<"+str(max_element_len+1)+"}"
        
    ret_str += format_string.format(*headers)+"\n"
    for c in ret_str:
        if c != " " and c != "\n":
            ret_str+= "-"
        else:
            ret_str += " "
    ret_str += "\n"
    for entry in data:
        ret_str += format_string.format(*entry) + "\n"
    return ret_str

def Dialog(_str, type=_eternalhush.ctxObj.STRING, default=None, console_id=None):
    """Requests arbitrary user input in the EternalHush console"""
    if isinstance(default, int):
        default = str(default)
        
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    resp = _eternalhush.ctxObj.dialog(console_id, _eternalhush.ctxObj.task_uid, _str, type, default)
    if type == _eternalhush.ctxObj.INT:
        resp = int(resp)
    return resp


def Option(_str, default=_eternalhush.ctxObj.OPTION_YES, console_id=None):
    """Requests confirmation of the operation in the EternalHush console"""
    _str = str(_str)
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.option(console_id, _eternalhush.ctxObj.task_uid, _str, default)


def Run(_str, _flag, console_id=None):
    """Executes the specified command in the EternalHush console"""
    _str = str(_str)
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.run(console_id, _eternalhush.ctxObj.task_uid, _str, _flag)


def Echo(_str, _type, console_id=None):
    """Outputs the specified text to the EternalHush console"""
    _str = str(_str)
    if _str[-1:] == "\n":
        _str = _str[:-1]
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.echo(console_id, _eternalhush.ctxObj.task_uid, _str, _type)
    
    
def SetHostname(_str, console_id=None):
    """Changes the hostname for the current session"""
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.set_hostname(console_id, _eternalhush.ctxObj.task_uid, _str)
    
def SetEnv(_str, _val, console_id=None):
    if isinstance(_val, int):
        _val = str(_val)
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.set_env(console_id, _eternalhush.ctxObj.task_uid, _str, _val)
    
def GetEnv(_str, console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.get_env(console_id,_eternalhush.ctxObj.task_uid, _str)
    
def SetGlobalEnv(_str, _val):
    if isinstance(_val, int):
        _val = str(_val)
    return _eternalhush.ctxObj.set_global_env(_str, _val)
    
def GetGlobalEnv(_str):
    return _eternalhush.ctxObj.get_global_env(_str)

def GetAvailableConsole():
    return _eternalhush.ctxObj.get_first_available()
    
def SetLastError(code):
    SetEnv("LAST_ERROR", str(code))
    return
    
def GetLastError():
    return int(GetEnv("LAST_ERROR"))
    
def GetLockedModule(console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.get_locked_module(console_id, _eternalhush.ctxObj.task_uid)

def LockModule(_str, console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.lock_session(console_id, _eternalhush.ctxObj.task_uid, _str)

def UnlockModule(console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.unlock_session(console_id, _eternalhush.ctxObj.task_uid)

def SuppressEcho(_type, console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.suppress_msg_type(console_id, _eternalhush.ctxObj.task_uid, _type)

def UnsuppressEcho(_type, console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.disable_suppress(console_id, _eternalhush.ctxObj.task_uid, _type)

def UseConsoleContext(cid, console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    _eternalhush.ctxObj.console_id = cid

def AddCryptoKey(key, key2, key_name, key_type, algorithm, console_id=None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    return _eternalhush.ctxObj.add_crypto_key(console_id, key, key2, key_name, key_type, algorithm)
 
def QueryKeyStorage(console_id = None):
    if console_id == None:
        console_id = _eternalhush.ctxObj.console_id
    keyset = _eternalhush.ctxObj.query_key_storage(console_id)
    ret_keyset = []
    for e in keyset:
        ke = e.split('$$KSEP$$')
        ret_keyset.append({'name' : ke[0], 'data' : ke[1], 'data2': ke[2], 'algorithm' : ke[3], 'type' : ke[4]})
    return ret_keyset