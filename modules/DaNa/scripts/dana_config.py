import eternalhush as eh
from additional.clingyspider import api, extapi
from additional.clingyspider import const
from additional import memrwlib
from additional.clingyspider.nt_const import *
import additional.darknarrator as dana
import os
import time


layers = {
    'throttlestop' : dana.THROTTLESTOP_LAYER
}

def main(args):
    if eh.ui.GetEnv("DANA_REGISTERED") != 'true':
        eh.ui.Echo("DarkNarrator extension is not registered!", eh.ECHO_ERROR)
        return

    if args.set_layer:
        if args.set_layer.lower() not in layers.keys():
            eh.ui.Echo(f"Invalid layer name '{args.set_layer}'", eh.ECHO_ERROR)
            return

        res = extapi.InvokeUserExtensionApi(dana.DANA_SELECT_LAYER, layers[args.set_layer.lower()])
        if res:
            eh.ui.Echo(f"OK", eh.ECHO_GOOD)
            eh.ui.SetEnv("DANA_LAYER_UID", str(layers[args.set_layer.lower()]))
    elif args.destroy_layer:
        if not eh.ui.GetEnv("DANA_LAYER_UID") or len(eh.ui.GetEnv("DANA_LAYER_UID")) < 16:
            eh.ui.Echo(f"Invalid active layer uid", eh.ECHO_ERROR)
            return

        res = extapi.InvokeUserExtensionApi(dana.DANA_FINALIZE_LAYER)
        if res:
            eh.ui.Echo(f"OK", eh.ECHO_GOOD)
            eh.ui.SetEnv("DANA_LAYER_UID", str(layers[args.set_layer.lower()]))
    else:
        eh.ui.Echo(f"Please specify an option!", eh.ECHO_ERROR)
        return