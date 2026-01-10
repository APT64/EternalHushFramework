import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time

def clear_env(name):
    eh.ui.SetEnv(name, '')

def main(args):
    api.UserSyscall("NtTerminateProcess", api.GetCurrentProcess(), 0)
    eh.ui.Echo("Implant process terminated", eh.ECHO_GOOD)

    if eh.ui.GetLockedModule() != "":
        eh.ui.UnlockModule()
    eh.ui.SetHostname(eh.ui.GetGlobalEnv("OPERATOR_HOSTNAME"))
    clear_env('MIBA_IV')
    clear_env('MIBA_CONNECTION_TYPE')
    clear_env('MIBA_CONNECTION')
    clear_env('MIBA_ARCH')
    clear_env('MIBA_VERSION')
    clear_env('MIBA_KEY')
    clear_env('BUILDNO_MAJOR')
    clear_env('BUILDNO_MINOR')
    clear_env('NTOS_MAJOR')
    clear_env('NTOS_MINOR')
    clear_env('PLATFORM_ARCH')
    eh.ui.Echo("Session cleaned up!", eh.ECHO_GOOD)