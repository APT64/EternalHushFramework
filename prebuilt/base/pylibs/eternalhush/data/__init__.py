from eternalhush.exception import *
import _eternalhush
import binascii
import re

def RoundToDiv(number, target_number):
    temp = number
    while temp % target_number:
        temp = temp + 1
    return temp-number

def IntToBytes(number: int) -> bytes:
    return number.to_bytes(length=(8 + (number + (number < 0)).bit_length()) // 8, byteorder='big', signed=True)

def BytesToInt(binary_data: bytes) -> int:
    return int.from_bytes(binary_data, byteorder='big', signed=True)

class _InOut_(object):
    arg = None
    _type = None
    def __init__(self, arg):
        self.set(arg)

    def set(self, value):
        if self._type != None and (type(value) != self._type):
            raise TypeError("Cannot change base type")
        
        if isinstance(value, str):
            self._type = str
        if isinstance(value, int):
            self._type = int
        if isinstance(value, bytes):
            self._type = bytes
        self.arg = value
    def is_str(self):
        return self._type == str
    def is_int(self):
        return self._type == int
    def is_bytes(self):
        return self._type == bytes
    def get(self):
        return self.arg

class _InOutStruct_(_InOut_):
    struct = None
    def __init__(self, struct_def):
        self.struct = Struct(struct_def)
    def set(self, value):
        self.struct.from_bytes(value)
    def get(self):
        return self.struct

class ArgList():
    def __setattr__(self, attrname, value):
        self.__dict__[attrname]= value
    def __getattr__(self, item):
        return None


class ArgParser():
    def __init__(self):
        self.arglist = ArgList()
    def parse(self):
        for arg in _eternalhush.ctxObj.args:
            self.arglist.__setattr__(arg[0][1:], arg[1])
        return self.arglist


#HIGHLY NOT RECOMMENDED TO USE!!!
def offsetof(instance, target_field: str):
    if not isinstance(instance, Struct):
        raise TypeError("non Struct object passed!")
    definition = instance._definition
    offset = 0
    keys = list(definition.keys())
    i = 0
    while i < len(keys):
        key = keys[i]
        size = definition[key]
        if key.startswith("$BF"):
            group_keys = []
            group_bits = []
            base_size = size         
            capacity = base_size * 8  
            total_bits = 0
            while i < len(keys) and keys[i].startswith("$BF"):
                curr_key = keys[i]
                curr_size = definition[curr_key]
                if curr_size != base_size:
                    break
                m = re.match(r"\$BF(\d+)_", curr_key)
                bits = int(m.group(1))
                if total_bits + bits > capacity:
                    break
                group_keys.append(curr_key)
                group_bits.append(bits)
                total_bits += bits
                i += 1
            bit_offset = 0
            for idx, full_name in enumerate(group_keys):
                m = re.match(r"\$BF(\d+)_", full_name)
                norm_name = full_name[4 + len(m.group(1)) :]
                if norm_name == target_field:
                    return (offset, bit_offset, base_size)
                bit_offset += group_bits[idx]
            offset += base_size
        else:
            if key == target_field:
                return (offset, 0)
            offset += size
            i += 1

def StructSize(definition):
    return len(Struct(definition))

class Struct(object):
    struct_data = 0
    struct_size = 0
    _definition = 0
    g_offset = 0

    def _make_sub_list(self, list, idx):
        localidx = idx
        local_list = {}
        i = 0
        for key, value in list.items():
            if i >= localidx:
                local_list[key] = value
            i+=1
        return local_list
    
    def calculate_bitfields_group_offset(self, first_entry):
        bitgroup = first_entry.bgroup()
        pass

    def calculate_bitfileds_size_and_count(self, fields: dict) -> int:
        total_size = 0
        count = 0
        current_group_bits = 0
        current_group_capacity = 0 
        current_group_field_size = None
        def flush_group():
            nonlocal total_size, current_group_bits, current_group_capacity, current_group_field_size
            if current_group_bits > 0:
                total_size += current_group_field_size
                current_group_bits = 0
                current_group_capacity = 0
                current_group_field_size = None
        for key, field_size in fields.items():
            if key.startswith("$BF"):
                count += 1
                m = re.match(r"\$BF(\d+)_", key)
                bits = int(m.group(1))
                capacity = field_size * 8
                if current_group_field_size is None:
                    current_group_field_size = field_size
                    current_group_capacity = capacity
                    current_group_bits = bits
                else:
                    if current_group_field_size == field_size:
                        if current_group_bits + bits <= current_group_capacity:
                            current_group_bits += bits
                        else:
                            flush_group()
                            current_group_field_size = field_size
                            current_group_capacity = capacity
                            current_group_bits = bits
                    else:
                        flush_group()
                        current_group_field_size = field_size
                        current_group_capacity = capacity
                        current_group_bits = bits
            else:
                flush_group()
                return total_size, count
            
        flush_group()
        return total_size, count
            
    def normalize_bitf_name(self, name):
        str_bits = re.match(r"\$BF(\d+)_", name).group(1)
        return name[4+len(str_bits):]
    
    def get_bits_from_field_name(self, name):
        str_bits = re.match(r"\$BF(\d+)_", name).group(1)
        return int(str_bits)


    def __init__(self, definition):
        object.__setattr__(self, "_definition", definition)
        object.__setattr__(self, "struct_size", 0)
        
        entry_idx = 0
        bitf_size = 0
        bitf_flag = True
        bitf_count = 0
        bitf_group = None
        bitf_group_id = 0
        for key, value in definition.items():
            if key not in self.__dict__:
                entry_size = value
                entry_bits = None
                if key.startswith('$BF'):
                    bitf_size, bitf_count = self.calculate_bitfileds_size_and_count(self._make_sub_list(definition, entry_idx))
                    if not bitf_flag:
                        entry_size = bitf_size
                    else:
                        entry_size = 0
                    bitf_group = bitf_group_id
                    entry_bits = self.get_bits_from_field_name(key)
                    key = self.normalize_bitf_name(key)
                    if not bitf_flag:
                        bitf_flag = True
                elif bitf_flag:
                    bitf_group = None
                    bitf_group_id+=1
                    bitf_flag = False
            
                if bitf_group == None:
                    entry_bits = None
                self.__dict__[key] = Field(entry_size, bitf_group=bitf_group, bits=entry_bits)
                self.struct_size += entry_size
                entry_idx+=1
        self.struct_data = bytearray(b"\x00" * self.struct_size)
    
    def __setattr__(self, attrname, value):
        if attrname in self._definition.keys():
            self.__dict__[attrname].set(value) #need to fix bitfields processing
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
        if len(self.struct_data) < self.struct_size:
            self.struct_data = self.struct_data + b'\x00' * (self.struct_size - len(self.struct_data))
        for key, size in self._definition.items():
            byte_value = bytearray(b"\x00" * size)
            for i in range(size):
                byte_value[i] = self.struct_data[i+offset+global_offset]
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

    def bgroup(self):
        return self.bitfgroup

    def __init__(self, size, bits = None, bitf_group = None):
        self.field_size = size
        self.bits = bits
        self.bitfgroup = bitf_group

    def __eq__(self, other):
        if isinstance(other, bytes):
            return self._item == other
        elif isinstance(other, bytearray):
            return self._item == other
        elif isinstance(other, str):
            return self._item.decode("utf-8") == other
        elif isinstance(other, int):
            return int.from_bytes(self._item, byteorder="little") == other
                
    def __ne__(self, other):
        if isinstance(other, bytes):
            return self._item != other
        elif isinstance(other, bytearray):
            return self._item != other
        elif isinstance(other, str):
            return self._item.decode("utf-8") != other
        elif isinstance(other, int):
            return int.from_bytes(self._item, byteorder="little") != other

    def __str__(self):
        return object.__str__(self._item)
    
    def __bytes__(self):
        if isinstance(self._item, int):
            return self._item.to_bytes(self.field_size, "little")
        if isinstance(self._item, str):
            return bytes(self._item, "utf-8")
        return bytes(self._item)
            
    def __int__(self):
        return self.get(int)
    
    def __len__(self):
        return self.field_size
    
    def __lt__(self, other):
        return int(self) < other

    def __gt__(self, other):
        return int(self) > other
    
    def __le__(self, other):
        return int(self) <= other

    def __ge__(self, other):
        return int(self) >= other

    def __add__(self, other):
        return int(self) + other

    def __radd__(self, other):
        return int(self) + other
    
    def __sub__(self, other):
        return int(self) - other
    
    def __mul__(self, other):
        return other * int(self)
    
    def __rmul__(self, other):
        return other * int(self)
    
    def __truediv__(self, other):
        return int(self) / other
    
    def __rtruediv__(self, other):
        return other / int(self)
    
    def __floordiv__(self, other):
        return int(self) // other
    
    def __rfloordiv__(self, other):
        return other // int(self)
    
    def __mod__(self, other):
        return int(self) % other
    
    def __rmod__(self, other):
        return other % int(self)
    
    def __pow__(self, other):
        return int(self)**other
    
    def __rpow__(self, other):
        return other**int(self)
    
    def set(self, value):
        self._item = value
            
    def get(self, tp=bytes, base=10, ord="little", enc="utf-8"):
        if isinstance(self._item, tp):
            return self._item
        if tp == int:
            if isinstance(self._item, bytes) or isinstance(self._item, bytearray):
                return int.from_bytes(self._item, ord)
            return int(self._item, base)
            
        elif tp == bytes:
            if isinstance(self._item, int):
                return self._item.to_bytes(self.field_size, ord)    
            if isinstance(self._item, bytes) or isinstance(self._item, bytearray):
                return bytes(self._item)
            return bytes(self._item, enc)
                
        elif tp == bytearray:
            if isinstance(self._item, int):
                return bytearray(self._item.to_bytes(self.field_size, ord))
            if isinstance(self._item, bytes) or isinstance(self._item, bytearray):
                return bytearray(self._item)
            return bytearray(self._item, enc)
        elif tp == str:
            if isinstance(self._item, int) or isinstance(self._item, str):
                return str(self._item, encoding=enc)
            return self._item.decode(enc).split("\x00")[0]

        return self._item
    
    