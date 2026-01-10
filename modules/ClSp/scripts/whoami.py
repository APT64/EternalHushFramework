import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os

def main(args):
    token = api.GetProcessToken(api.GetCurrentProcess(), const.TOKEN_QUERY)
    uid = api.TokenGetUID(token)
    if uid != None:
        eh.ui.Echo(uid, eh.ECHO_DEFAULT)
    api.CloseFile(token)
    


