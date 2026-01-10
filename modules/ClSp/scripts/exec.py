import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time

def main(args):
    retn = api.CreateProcess("cmd.exe", "/c "+args.cmd, 0)
    if not retn:
        eh.ui.Echo("Failed to create remote process 'cmd.exe /c {}'".format(args.cmd), eh.ECHO_ERROR)
        return
    if args.timeout:
        time.sleep(args.timeout)
    else:
        time.sleep(5)
    read_handle = retn.hread.get(int)
    write_handle = retn.hwrite.get(int)
    process_handle = retn.hprocess.get(int)    
    thread_handle = retn.hthread.get(int)
    
    eh.ui.Echo("Created remote process 'cmd.exe /c {}'".format(args.cmd), eh.ECHO_GOOD)
    eh.ui.Echo("============CAPTURED OUTPUT=================", eh.ECHO_DEFAULT)
    
    text = api.ReadFile(read_handle).decode("cp866", errors="replace")
    if text == None:
        eh.ui.Echo(f"Failed to read from remote pipe")
        return
    eh.ui.Echo(text, eh.ECHO_DEFAULT)
    
    api.CloseFile(write_handle)
    api.CloseFile(read_handle)
    api.CloseFile(thread_handle)
    api.CloseFile(process_handle)
    
    


