import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import xml.etree.cElementTree as ET
import xml.dom.minidom

def main(args):
    hkey = api.RegOpenKey(const.HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", const.KEY_READ)
    DisplayVersion = api.RegQueryValue(hkey, "DisplayVersion").data.get()[:-2].decode('utf-16-le')
    InstallationType = api.RegQueryValue(hkey, "InstallationType").data.get()[:-2].decode('utf-16-le')

    ProductName = api.RegQueryValue(hkey, "ProductName").data.get()[:-2].decode('utf-16-le')
    CurrentBuild = api.RegQueryValue(hkey, "CurrentBuild").data.get()[:-2].decode('utf-16-le')
    buildinfoex = api.RegQueryValue(hkey, "BuildLabEx").data.get()[:-2].decode('utf-16-le')
    api.CloseHandle(hkey)

    eh.ui.Echo("NT:                {}.{}".format(eh.ui.GetEnv('NTOS_MAJOR'), eh.ui.GetEnv('NTOS_MINOR')), eh.ECHO_DEFAULT)
    eh.ui.Echo("OS:                {} {} ({})".format(ProductName, DisplayVersion, CurrentBuild), eh.ECHO_DEFAULT)
    eh.ui.Echo("INSTALLATION TYPE: {}".format(InstallationType), eh.ECHO_DEFAULT)
    eh.ui.Echo("BUILD INFO EX:     {}".format(buildinfoex), eh.ECHO_DEFAULT)
    eh.ui.Echo("BUILD NUMBER:      {}.{}".format(eh.ui.GetEnv('BUILDNO_MAJOR'), eh.ui.GetEnv('BUILDNO_MINOR')), eh.ECHO_DEFAULT)
    eh.ui.Echo("PLATFORM ARCH:     {}".format(eh.ui.GetEnv('PLATFORM_ARCH')), eh.ECHO_DEFAULT)
    eh.ui.Echo("COMPUTER NAME:     {}".format(api.GetEnvVar('COMPUTERNAME')), eh.ECHO_DEFAULT)
    eh.ui.Echo("SYSTEM DRIVE:      {}".format(api.GetEnvVar('SYSTEMDRIVE')), eh.ECHO_DEFAULT)
    eh.ui.Echo("PROCESSOR CORES:   {}".format(api.GetEnvVar('NUMBER_OF_PROCESSORS')), eh.ECHO_DEFAULT)


    