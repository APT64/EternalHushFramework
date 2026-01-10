import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os


def main(args):
    if api.SetVirtualWorkdir(args.path):
        eh.ui.Echo("Current virtual working directory changed to:", eh.ECHO_DEFAULT)
        eh.ui.Echo(args.path, eh.ECHO_DEFAULT)




