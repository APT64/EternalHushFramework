from eternalhush.exception import *
import _eternalhush

class Struct(object):
    def __init__(self, definition):
        global struct_data
        global struct_size
        global init_completed
        global _definition
        
        struct_size = 0
        _definition = definition
        
        for key, value in definition.items():
            if key not in self.__dict__:
                self.__dict__[key] = 0
                struct_size += value
        struct_data = bytearray(b"\x00" * struct_size)
        init_completed = 1
        
    def __setattr__(self, attrname, value):
        if attrname in self.__dict__ and attrname in _definition:
            self.__dict__[attrname] = value
        else:
            raise AttributeNotFound(attrname)
        
        if init_completed:
            offset = 0
            for key, val in _definition.items():
                if attrname == key:
                    break
                offset += val
            if isinstance(value, int):
                value = value.to_bytes(_definition.get(attrname), byteorder="little")            
            elif isinstance(value, bytes):
                value = value[::-1]
            for i in range(len(value)):
                struct_data[i+offset] = value[i] 
    
    def data(self):
        return struct_data
    
    def from_bytes(self, array):
        offset = 0
        for key, size in _definition.items():
            byte_value = bytearray(b"\x00" * size)
            for i in range(size):
                byte_value[i] = array[i+offset]
                struct_data[i+offset] = array[i+offset]
            offset += size
            self.__dict__[key] = byte_value
    
    def __sizeof__(self):
        return struct_size
   
    def __len__(self):
        return struct_size