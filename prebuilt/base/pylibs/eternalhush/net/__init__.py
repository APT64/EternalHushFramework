import _eternalhush

def NewConnection(addr, port, type):
    """Establishes and provides access to a direct network connection"""
    return _eternalhush.ctxObj.create_new_connection(addr, port, type)
    
def TcpSend(id, data):
    """Sends a TCP packet over the specified network connection"""
    _eternalhush.ctxObj.tcp_send(id, data)
    return None
    
def TcpRecv(id, size):
    """Receiving a TCP packet over the specified network connection"""
    return _eternalhush.ctxObj.tcp_recv(id, size)