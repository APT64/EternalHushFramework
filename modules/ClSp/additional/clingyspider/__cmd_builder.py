import eternalhush as eh
class CommandBuilder:
    def __init__(self):
        self.cmd_code = 0
        self.result_bytes = bytearray()
        self.args_bytes = bytearray()
        
    def set_command(self, code):
        self.cmd_code = code
        
    def add_long(self, arg: int):
        sig = False
        if arg < 0:
            sig = True
        self.args_bytes += arg.to_bytes(8, "big", signed=sig)     

    def add_int(self, arg: int):
        sig = False
        if arg < 0:
            sig = True
        self.args_bytes += arg.to_bytes(4, "big", signed=sig)        
    
    def add_short(self, arg: int):
        sig = False
        if arg < 0:
            sig = True
        self.args_bytes += arg.to_bytes(2, "big", signed=sig)
        
    def add_byte(self, arg: int):
        byte = arg & 0xff
        sig = False
        if arg < 0:
            sig = True
        self.args_bytes += byte.to_bytes(1, "big", signed=sig)
    
    def add_strarg(self, arg: str, length=None):
        arg_len = len(arg)
        if length != None:
            arg_len = length
        if arg_len == 0:
            self.add_int(0)
        else:
            self.add_int(arg_len+1)
            self.add_str(arg)

    def add_wstrarg(self, arg: str, length=None):
        arg_len = len(arg.encode("utf-16-le"))
        if length != None:
            arg_len = length
        if arg_len == 0:
            self.add_int(0)
        else:
            self.add_int(arg_len+2)
            self.add_wstr(arg)
            
    def add_bstrarg(self, arg: bytes, length=None):
        arg_len = len(arg)
        if length != None:
            arg_len = length
        if arg_len == 0:
            self.add_int(0)
        else:
            self.add_int(arg_len)
            self.add_bstr(arg)

    def add_bstr(self, arg: bytes):
        self.args_bytes += arg 
        
    def add_str(self, arg: str):
        self.args_bytes += arg.encode() 
        self.add_byte(0)

    def add_wstr(self, arg: str):
        self.args_bytes += arg.encode("utf-16-le") 
        self.add_byte(0)
        self.add_byte(0)
    
    def padding(self, div):
        while len(self.args_bytes)+1 % div != 0:
            self.add_byte(0)
    
    def build(self):
        try:
            self.result_bytes = self.cmd_code.to_bytes(1, "big")
            self.result_bytes += self.args_bytes
            self.args_bytes = b""
            return self.result_bytes
        except Exception:
            return None
    
    def build_nocmd(self):
        try:
            self.result_bytes = self.args_bytes
            self.args_bytes = b""
            return self.result_bytes
        except Exception:
            return None