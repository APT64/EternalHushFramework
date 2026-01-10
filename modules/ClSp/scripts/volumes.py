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

    eh.ui.Echo("Collecting hardware info...", eh.ECHO_DEFAULT)
    parsed_disksinfo = query_class_info(wmi_id, "Win32_Volume", ["BootVolume", "Capacity", "DriveLetter", "DeviceID", 'FileSystem', 'FreeSpace', 'Label', 'SystemVolume'])

    xmlroot = ET.Element("SystemVolumes")

    for entry in parsed_disksinfo:
        physmedia_xml = ET.SubElement(xmlroot, "Volume")
        ET.SubElement(physmedia_xml, "Letter").text =  str(entry['DriveLetter'])
        ET.SubElement(physmedia_xml, "Label").text =  str(entry['Label'])
        ET.SubElement(physmedia_xml, "DevID").text =  str(entry['DeviceID'])
        ET.SubElement(physmedia_xml, "Bootable").text =  str(entry['BootVolume'])
        ET.SubElement(physmedia_xml, "Capacity").text =  str(entry['Capacity'])
        ET.SubElement(physmedia_xml, "FreeSpace").text =  str(entry['FreeSpace'])
        ET.SubElement(physmedia_xml, "FileSystem").text =  str(entry['FileSystem'])
        ET.SubElement(physmedia_xml, "IsSystem").text =  str(entry['SystemVolume'])

        eh.ui.Echo("Letter:\t\t" + str(entry['DriveLetter']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Label:\t\t" + str(entry['Label']), eh.ECHO_DEFAULT)
        eh.ui.Echo("DevID:\t\t" + str(entry['DeviceID']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Bootable:\t" + str(bool(entry['BootVolume'])), eh.ECHO_DEFAULT)
        eh.ui.Echo("Capacity:\t" + str(int(entry['Capacity'], 10)//(1024*1024)) + " MB", eh.ECHO_DEFAULT)
        eh.ui.Echo("FreeSpace:\t" + str(int(entry['FreeSpace'], 10)//(1024*1024)) + " MB", eh.ECHO_DEFAULT)
        eh.ui.Echo("FileSystem:\t" + str(entry['FileSystem']), eh.ECHO_DEFAULT)
        eh.ui.Echo("IsSystem:\t" + str(bool(entry['SystemVolume'])), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    api.WmiRelease(wmi_id)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")
