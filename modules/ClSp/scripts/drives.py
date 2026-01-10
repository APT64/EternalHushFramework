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

    parsed_disksinfo = query_class_info(wmi_id, "Win32_DiskDrive", ["DeviceID", "Caption", "Size", "Partitions", 'SerialNumber', 'FirmwareRevision', 'PNPDeviceID', 'MediaType'])

    xmlroot = ET.Element("MediaDevices")

    for entry in parsed_disksinfo:
        physmedia_xml = ET.SubElement(xmlroot, "Device")
        ET.SubElement(physmedia_xml, "Name").text =  str(entry['Caption'])
        ET.SubElement(physmedia_xml, "DevID").text =  str(entry['DeviceID'])
        ET.SubElement(physmedia_xml, "Serial").text =  str(entry['SerialNumber'])
        ET.SubElement(physmedia_xml, "Firmware").text =  str(entry['FirmwareRevision'])
        ET.SubElement(physmedia_xml, "Type").text =  str(entry['MediaType'])
        ET.SubElement(physmedia_xml, "PnPDevID").text =  str(entry['PNPDeviceID'])
        ET.SubElement(physmedia_xml, "PartitionCount").text =  str(entry['Partitions'])
        ET.SubElement(physmedia_xml, "Size").text =  str(entry['Size'])

        eh.ui.Echo("Caption:\t\t" + str(entry['Caption']), eh.ECHO_DEFAULT)
        eh.ui.Echo("DeviceID:\t" + str(entry['DeviceID']), eh.ECHO_DEFAULT)
        eh.ui.Echo("SN:\t\t" + str(entry['SerialNumber']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Firmware:\t" + str(entry['FirmwareRevision']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Type:\t\t" + str(entry['MediaType']), eh.ECHO_DEFAULT)
        eh.ui.Echo("PNPDeviceID:\t" + str(entry['PNPDeviceID']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Partitions:\t" + str(entry['Partitions']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Size:\t\t" + str(int(entry['Size'], 10)//(1024*1024)) + " MB", eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    api.WmiRelease(wmi_id)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")
