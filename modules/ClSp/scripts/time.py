import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const

def main(args):
    ptr = api.LocalAlloc(8)
    status = api.UserSyscall("NtQuerySystemTime", ptr)