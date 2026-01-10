import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os

def main(args):
    result = api.BlockDlls(args.state)
    if result:
        eh.ui.Echo("Non-Microsoft dll loading successfully blocked", eh.ECHO_GOOD)
    else:
        eh.ui.Echo("Failed to block non-Microsoft dll loading", eh.ECHO_ERROR)
    


