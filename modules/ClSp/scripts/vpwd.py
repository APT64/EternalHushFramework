import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os


def main(args):
    string = api.GetVirtualWorkdir()
    eh.ui.Echo("Current virtual working directory:", eh.ECHO_DEFAULT)
    eh.ui.Echo(string, eh.ECHO_DEFAULT)



