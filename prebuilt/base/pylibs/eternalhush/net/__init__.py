import _eternalhush

def NewConnection(addr, port, type):
    """Establishes and provides access to a direct network connection"""
    return _eternalhush.ctxObj.create_new_connection(addr, port, type)
    
def LockSession(_str):
    return _eternalhush.ctxObj.lock_session(_eternalhush.ctxObj.console_id, _str)
    
def TcpSend(id, data):
    """Sends a TCP packet over the specified network connection"""
    _eternalhush.ctxObj.tcp_send(id, data)
    return None
    
def TcpRecv(id, size):
    """Receiving a TCP packet over the specified network connection"""
    return _eternalhush.ctxObj.tcp_recv(id, size)
    
def AddHandler(id, path, name):
    """Receiving a TCP packet over the specified network connection"""
    return _eternalhush.ctxObj.add_exception_handler(id, path, name)