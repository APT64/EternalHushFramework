import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time
import xml.etree.cElementTree as ET
import xml.dom.minidom

def query_class_info(wmi_id, classname, filter):
    if not api.WmiQuery(wmi_id, "SELECT * FROM " + classname):
        eh.ui.Echo("Failed to query {classname} class", eh.ECHO_ERROR)
        return
    return api.WmiParseResult(wmi_id, filter)


def main(args):
    wmi_id = api.WmiInitialize()
    if wmi_id == None:
        eh.ui.Echo("Failed to initialize wmi session", eh.ECHO_ERROR)
        return
    if not api.WmiConnect(wmi_id, "root\\CIMV2"):
        eh.ui.Echo("Failed to connect to wmi resource 'root\\CIMV2'", eh.ECHO_ERROR)
        return

    parsed_driverinfo = query_class_info(wmi_id, "Win32_SystemDriver", ["DisplayName", "Name", "ServiceType", 'PathName', 'AcceptPause', 'AcceptStop', 'State', 'StartMode'])

    xmlroot = ET.Element("TargetDrivers")

    for entry in parsed_driverinfo:
        physmedia_xml = ET.SubElement(xmlroot, "Driver")
        ET.SubElement(physmedia_xml, "Name").text =  str(entry['Name'])
        ET.SubElement(physmedia_xml, "DisplayName").text =  str(entry['DisplayName'])
        ET.SubElement(physmedia_xml, "BinPath").text =  str(entry['PathName'])
        ET.SubElement(physmedia_xml, "ServiceType").text =  str(entry['ServiceType'])
        ET.SubElement(physmedia_xml, "Pausable").text =  str(bool(entry['AcceptPause']))
        ET.SubElement(physmedia_xml, "Stoppable").text =  str(bool(entry['AcceptStop']))
        ET.SubElement(physmedia_xml, "State").text =  str(entry['State'])
        ET.SubElement(physmedia_xml, "StartMode").text =  str(entry['StartMode'])
        eh.ui.Echo("-----------------------------------------------------", eh.ECHO_DEFAULT)
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("DisplayName:\t" + str(entry['DisplayName']), eh.ECHO_DEFAULT)
        eh.ui.Echo("BinPath:\t\t" + str(entry['PathName']), eh.ECHO_DEFAULT)
        options = ""
        if entry['AcceptStop']:
            options += "Stoppable "
        if entry['AcceptPause']:
            options += "Pausable "
        eh.ui.Echo("Options:\t\t" + options, eh.ECHO_DEFAULT)
        eh.ui.Echo("State:\t\t" + str(entry['State']), eh.ECHO_DEFAULT)
        eh.ui.Echo("StartMode:\t" + str(entry['StartMode']), eh.ECHO_DEFAULT)

    api.WmiRelease(wmi_id)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")
