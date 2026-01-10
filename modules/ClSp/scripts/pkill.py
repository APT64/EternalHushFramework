import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const

def main(args):
    if args.pid == 0 or args.pid == 4:
        eh.ui.Echo("Cannot terminate system processes!", eh.ECHO_ERROR)
        return
    
    if args.code == None:
        args.code = 0

    handle = api.GetProcessHandle(args.pid, const.PROCESS_TERMINATE)
    status = api.UserSyscall("NtTerminateProcess", handle, args.code)
    if status == 0:
        eh.ui.Echo("Target process terminated", eh.ECHO_GOOD)
    else:
        eh.ui.Echo("Process termination failed", eh.ECHO_ERROR)