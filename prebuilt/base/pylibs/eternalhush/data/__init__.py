from eternalhush.exception import *
import _eternalhush
import binascii

class Struct(object):
    struct_data = 0
    struct_size = 0
    _definition = 0
    g_offset = 0

    def __init__(self, definition):
        object.__setattr__(self, "_definition", definition)
        object.__setattr__(self, "struct_size", 0)
        
        for key, value in definition.items():
            if key not in self.__dict__:
                self.__dict__[key] = Field(value)
                self.struct_size += value
        self.struct_data = bytearray(b"\x00" * self.struct_size)
        
    def __setattr__(self, attrname, value):
        if attrname in self._definition.keys():
            self.__dict__[attrname].set(value)
            offset = 0
            for key, val in self._definition.items():
                if attrname == key:
                    break
                offset += val
            if isinstance(value, int):
                value = value.to_bytes(self._definition.get(attrname), byteorder="little")            
            elif isinstance(value, bytes):
                value = value
            elif isinstance(value, bytearray):
                value = value
            elif isinstance(value, str):
                value = bytes(value, 'utf-8')
            for i in range(len(value)):
                self.struct_data[i+offset+self.g_offset] = value[i] 
        else:
            self.__dict__[attrname] = value
    
    def data(self):
        return self.struct_data[self.g_offset:self.g_offset+self.struct_size]
    
    def full_data(self):
        return self.struct_data
    
    def from_bytes(self, array, global_offset=0):
        offset = 0
        self.g_offset = global_offset
        self.struct_data = bytearray(array)
        for key, size in self._definition.items():
            byte_value = bytearray(b"\x00" * size)
            for i in range(size):
                byte_value[i] = array[i+offset+global_offset]
            offset += size
            self.__dict__[key].set(byte_value)
    
    def __getattribute__(self, attrname):
        attr = object.__getattribute__(self, attrname)
        if isinstance(attr, Field):
            return self.__dict__[attrname]
        return attr
    
    def __sizeof__(self):
        return self.struct_size
   
    def __len__(self):
        return self.struct_size


class Field(object):
    field_size = 0
    text_encoding = "utf-8"
    
    def __init__(self, size):
        self.field_size = size

    def __eq__(self, other):
        if isinstance(other, bytes):
            return self._item == other
        elif isinstance(other, bytearray):
            return self._item == other
        elif isinstance(other, str):
            return self._item.decode("utf-8") == other
        elif isinstance(other, int):
            return int.from_bytes(self._item, byteorder="big") == other
                
    def __str__(self):
        if isinstance(self._item, int) or isinstance(self._item, str):
            return str(self._item)
        return self._item.decode("utf-8")
            
    
    def set(self, value):
        if isinstance(value, bytes):
            self._item = value[:self.field_size]
        elif isinstance(value, bytearray):
            self._item = bytes(value)[:self.field_size]
        elif isinstance(value, str):
            self._item = bytes(value, 'utf-8')[:self.field_size]
        elif isinstance(value, int):
            self._item = value.to_bytes(len(str(value)), byteorder="big")[len(str(value))-self.field_size:]
        else:
            raise Exception("Struct field must be of type int, bytes or str")
            
    def get(self, tp=None, base=10, ord="little", enc="utf-8"):
        if tp == int:
            if isinstance(self._item, bytes) or isinstance(self._item, bytearray):
                return int.from_bytes(self._item, ord)
            return int(self._item, base)
            
        elif tp == bytes:
            if isinstance(self._item, int):
                return self._item.to_bytes(filed_size, ord)    
            if isinstance(self._item, bytes) or isinstance(self._item, bytearray):
                return bytes(self._item)
            return bytes(self._item, enc)
                
        elif tp == bytearray:
            if isinstance(self._item, int):
                return bytearray(self._item.to_bytes(filed_size, ord))
            if isinstance(self._item, bytes) or isinstance(self._item, bytearray):
                return bytearray(self._item)
            return bytearray(self._item, enc)
        elif tp == str:
            if isinstance(self._item, int) or isinstance(self._item, str):
                return str(self._item, encoding=enc)
            return self._item.decode(enc)

        return self._item