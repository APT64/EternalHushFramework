#import eternalhush as eh
class ResponseParser:
    def __init__(self):
        self.resp_bytes = 0

    def load_data(self, data):
        self.resp_bytes = data

    def get_byte(self):
        byte = int.from_bytes(self.resp_bytes[:1], 'big')
        self.resp_bytes = self.resp_bytes[1:]
        return byte
    
    def get_short(self):
        byte = int.from_bytes(self.resp_bytes[:2], 'big')
        self.resp_bytes = self.resp_bytes[2:]
        return byte

    def get_int(self):
        byte = int.from_bytes(self.resp_bytes[:4], 'big')
        self.resp_bytes = self.resp_bytes[4:]
        return byte
    
    def get_long(self):
        byte = int.from_bytes(self.resp_bytes[:8], 'big')
        self.resp_bytes = self.resp_bytes[8:]
        return byte
    
    def get_str(self, len):
        try:
            byte = self.resp_bytes[:len].decode()
        except UnicodeDecodeError:
            byte = self.resp_bytes[:len].decode('cp1251')
        self.resp_bytes = self.resp_bytes[len:]
        return byte
    
    def get_wstr(self, len):
        byte = self.resp_bytes[:len].decode("utf-16-le")
        self.resp_bytes = self.resp_bytes[len:]
        return byte
    
    def get_bstr(self, len):
        byte = self.resp_bytes[:len]
        self.resp_bytes = self.resp_bytes[len:]
        return byte
    
    def get_strarg(self):
        len = self.get_int()
        return self.get_str(len)
    
    def get_wstrarg(self):
        len = self.get_int()
        return self.get_wstr(len)

    def get_bstrarg(self):
        len = self.get_int()
        return self.get_bstr(len)
     
    def dump(self):
        return self.resp_bytes