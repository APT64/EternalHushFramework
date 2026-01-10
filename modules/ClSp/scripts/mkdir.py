import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time

def main(args):
    dir_path = args.path.replace("/", "\\")
    drive_letter = dir_path[:2]
    attr = const.FILE_ATTRIBUTE_NORMAL
    if args.hidden:
        attr |= const.FILE_ATTRIBUTE_HIDDEN
    handle = api.CreateDirectory(dir_path, const.GENERIC_READ, const.FILE_CREATE, attr)
    if handle:
        api.CloseHandle(handle)
