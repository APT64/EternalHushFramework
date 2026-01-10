import eternalhush as eh
from additional import silentnight
import os

def main(args):
    process = 0
    if args.mode not in ["shellcode", "dll", "sleep", "alertable_sleep"]:
        eh.ui.Echo("Invalid mode specified!")
        return
    if args.arch.lower() not in ["win64", "win32"]:
        eh.ui.Echo("Invalid architecture specified!")
        return
    host_process = silentnight.HostProcess(args.arch.lower())
    if args.mode == "sleep":
        process = host_process.spawn_sleep()
    if args.mode == "alertable_sleep":
        process = host_process.spawn_alertable()
    if args.mode == "shellcode":
        if not args.bin or not os.path.isfile(args.bin):
            eh.ui.Echo("Invalid target file path specified!", eh.ECHO_ERROR)
            return
        process = host_process.spawn_shellcode(args.bin)
    if args.mode == "dll":
        if not args.bin or not os.path.isfile(args.bin):
            eh.ui.Echo("Invalid target file path specified!", eh.ECHO_ERROR)
            return
        
        if args.export != None and args.ordinal != None:
            eh.ui.Echo("You must specify either \"ordinal\" or \"export\"", eh.ECHO_ERROR)
            return
        if args.export:
            process = host_process.spawn_dll(args.bin, export=args.export)
        elif args.ordinal:
            process = host_process.spawn_dll(args.bin, export=args.ordinal)
        else:
            process = host_process.spawn_dll(args.bin)
    if process:
        eh.ui.Echo("Spawned hostprocess ({})".format(process.pid), eh.ECHO_GOOD)
    else:
        eh.ui.Echo("Failed to spawn hostprocess (code {})".format(process.returncode), eh.ECHO_ERROR)

    if args.timeout != None:
        pass #FIX!
        