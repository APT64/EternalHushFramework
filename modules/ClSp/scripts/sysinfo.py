import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import xml.etree.cElementTree as ET
import xml.dom.minidom

def main(args):
    eh.ui.Echo("WINDOWS:         {}.{}".format(eh.ui.GetEnv('NTOS_MAJOR'), eh.ui.GetEnv('NTOS_MINOR')), eh.ECHO_DEFAULT)
    eh.ui.Echo("BUILD NUMBER:    {}.{}".format(eh.ui.GetEnv('BUILDNO_MAJOR'), eh.ui.GetEnv('BUILDNO_MINOR')), eh.ECHO_DEFAULT)
    eh.ui.Echo("PLATFORM ARCH:   {}".format(eh.ui.GetEnv('PLATFORM_ARCH')), eh.ECHO_DEFAULT)
    eh.ui.Echo("COMPUTER NAME:   {}".format(api.GetEnvVar('COMPUTERNAME')), eh.ECHO_DEFAULT)
    eh.ui.Echo("SYSTEM DRIVE:    {}".format(api.GetEnvVar('SYSTEMDRIVE')), eh.ECHO_DEFAULT)
    eh.ui.Echo("PROCESSOR CORES: {}".format(api.GetEnvVar('NUMBER_OF_PROCESSORS')), eh.ECHO_DEFAULT)