import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time

def main(args):
    dir_path = args.path.replace("/", "\\")
    drive_letter = dir_path[:2]
    handle = api.CreateFileEx(dir_path,  const.GENERIC_READ | const.GENERIC_WRITE | const.DELETE, const.FILE_OPEN, const.FILE_ATTRIBUTE_NORMAL, const.FILE_DELETE_ON_CLOSE)
    if handle:
        api.CloseHandle(handle)
