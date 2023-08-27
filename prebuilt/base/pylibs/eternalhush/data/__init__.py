from eternalhush.exception import *
import _eternalhush

class Struct(object):
    struct_data = 0
    struct_size = 0
    _definition = 0
    g_offset = 0

    def __init__(self, definition):
        self._definition = definition
        self.struct_size = 0
        
        for key, value in definition.items():
            if key not in self.__dict__:
                self.__dict__[key] = 0
                self.struct_size += value
        self.struct_data = bytearray(b"\x00" * self.struct_size)
        
    def __setattr__(self, attrname, value):
        self.__dict__[attrname] = value

        if attrname in self._definition.keys():
            offset = 0
            for key, val in self._definition.items():
                if attrname == key:
                    break
                offset += val
            if isinstance(value, int):
                value = value.to_bytes(self._definition.get(attrname), byteorder="little")            
            elif isinstance(value, bytes):
                value = value[::-1]
            for i in range(len(value)):
                self.struct_data[i+offset+self.g_offset] = value[i] 
    
    def data(self):
        return self.struct_data[self.g_offset:self.g_offset+self.struct_size]
    
    def full_data(self):
        return self.struct_data
    
    def from_bytes(self, array, global_offset=0):
        offset = 0
        self.g_offset = global_offset
        self.struct_data = array
        for key, size in self._definition.items():
            byte_value = bytearray(b"\x00" * size)
            for i in range(size):
                byte_value[i] = array[i+offset+global_offset]
            offset += size
            self.__dict__[key] = byte_value
    
    def __sizeof__(self):
        return self.struct_size
   
    def __len__(self):
        return self.struct_size