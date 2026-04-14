import eternalhush as eh
from additional.clingyspider import api, extapi
from additional.clingyspider import const
from additional import memrwlib
from additional.clingyspider.nt_const import *
import additional.darknarrator as dana
import os
import time

def main(args):
    if not dana.DANA_IsReady():
        eh.ui.Echo("DarkNarrator is not loaded/configured, aborting...", eh.ECHO_ERROR)

    