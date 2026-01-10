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
    parsed_cpuinfo = query_class_info(wmi_id, "Win32_Processor", ["DeviceID", "Name"])
    parsed_adapterinfo = query_class_info(wmi_id, "Win32_NetworkAdapter", ["ServiceName", "MACAddress", "AdapterType", "DeviceID", "Name"])
    parsed_motherboardinfo = query_class_info(wmi_id, "Win32_BaseBoard", ["Manufacturer", "Model", "SerialNumber", "Product"])
    parsed_biosinfo = query_class_info(wmi_id, "Win32_BIOS", ["SMBIOSBIOSVersion", "Manufacturer", "Name", "SerialNumber", "Version"])
    parsed_vcardinfo = query_class_info(wmi_id, "Win32_VideoController", ["Name", "AdapterCompatibility", "DriverVersion", "CurrentHorizontalResolution", "CurrentVerticalResolution", "AdapterRAM", "PNPDeviceID"])
    parsed_disksinfo = query_class_info(wmi_id, "Win32_DiskDrive", ["DeviceID", "Caption", "Size", "Partitions"])
    parsed_sounddeviceinfo = query_class_info(wmi_id, "Win32_SoundDevice", ["Manufacturer", "Name"])
    parsed_desktopmonitorinfo = query_class_info(wmi_id, "Win32_DesktopMonitor", ["DeviceID", "Name", "MonitorManufacturer", "ScreenHeight", "ScreenWidth"])
    parsed_printerinfo = query_class_info(wmi_id, "Win32_Printer", ["Name", "SystemName"])

    xmlroot = ET.Element("HardwareInfo")
    cpulist_xml = ET.SubElement(xmlroot, "CPUInfo")
    vidlist_xml = ET.SubElement(xmlroot, "VideoControllerInfo")
    monitorlist_xml = ET.SubElement(xmlroot, "MonitorInfo")
    netadapterlist_xml = ET.SubElement(xmlroot, "NetAdapterInfo")
    soundlist_xml = ET.SubElement(xmlroot, "SoundDeviceInfo")
    printerlist_xml = ET.SubElement(xmlroot, "PrinterDeviceInfo")

    eh.ui.Echo("===============[MOTHERBOARD INFO]===============", eh.ECHO_DEFAULT)
    for entry in parsed_motherboardinfo:
        motherboard_xml = ET.SubElement(xmlroot, "MotherBoard")
        ET.SubElement(motherboard_xml, "Vendor").text =  str(entry['Manufacturer'])
        ET.SubElement(motherboard_xml, "Model").text =  str(entry['Model'])
        ET.SubElement(motherboard_xml, "Serial").text =  str(entry['SerialNumber'])
        ET.SubElement(motherboard_xml, "Product").text =  str(entry['Product'])
        eh.ui.Echo("Manufacturer:\t" + str(entry['Manufacturer']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Model:\t\t" + str(entry['Model']), eh.ECHO_DEFAULT)
        eh.ui.Echo("SN:\t\t" + str(entry['SerialNumber']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Product:\t\t" + str(entry['Product']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)
    
    eh.ui.Echo("===============[BIOS INFO]===============", eh.ECHO_DEFAULT)
    for entry in parsed_biosinfo:
        bios_xml = ET.SubElement(xmlroot, "Bios")
        ET.SubElement(bios_xml, "SMBIOSVersion").text =  str(entry['SMBIOSBIOSVersion'])
        ET.SubElement(bios_xml, "Vendor").text =  str(entry['Manufacturer'])
        ET.SubElement(bios_xml, "Name").text =  str(entry['Name'])
        ET.SubElement(bios_xml, "Serial").text =  str(entry['SerialNumber'])
        ET.SubElement(bios_xml, "Version").text =  str(entry['Version'])
        eh.ui.Echo("SMBIOS Ver:\t" + str(entry['SMBIOSBIOSVersion']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Manufacturer:\t" + str(entry['Manufacturer']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("SerialNumber:\t" + str(entry['SerialNumber']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Version:\t\t" + str(entry['Version']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    eh.ui.Echo("===============[CPU INFO]===============", eh.ECHO_DEFAULT)
    for entry in parsed_cpuinfo:
        cpu_xml = ET.SubElement(cpulist_xml, "Cpu")
        ET.SubElement(cpu_xml, "CpuID").text =  str(entry['DeviceID'])
        ET.SubElement(cpu_xml, "Name").text =  str(entry['Name'])
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("DeviceID:\t" + str(entry['DeviceID']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)
    
    eh.ui.Echo("===============[GRAPHICS INFO]===============", eh.ECHO_DEFAULT)
    for entry in parsed_vcardinfo:
        vid_xml = ET.SubElement(vidlist_xml, "VideoController")
        ET.SubElement(vid_xml, "Vendor").text =  str(entry['AdapterCompatibility'])
        ET.SubElement(vid_xml, "Name").text =  str(entry['Name'])
        ET.SubElement(vid_xml, "DriverVersion").text =  str(entry['DriverVersion'])
        ET.SubElement(vid_xml, "RAM").text =  str(entry['AdapterRAM'])
        ET.SubElement(vid_xml, "ResolutionX").text =  str(entry['CurrentHorizontalResolution'])
        ET.SubElement(vid_xml, "ResolutionY").text =  str(entry['CurrentVerticalResolution'])
        ET.SubElement(vid_xml, "PnPDevID").text =  str(entry['PNPDeviceID'])
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Compatibility:\t" + str(entry['AdapterCompatibility']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Driver Ver:\t" + str(entry['DriverVersion']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Resolution:\t" + str(entry['CurrentHorizontalResolution'])+"x" + str(entry['CurrentVerticalResolution']), eh.ECHO_DEFAULT)
        eh.ui.Echo("RAM:\t\t" + str(entry['AdapterRAM']//(1024*1024)) + " MB", eh.ECHO_DEFAULT)
        eh.ui.Echo("PnP DevID:\t" + str(entry['PNPDeviceID']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    eh.ui.Echo("===============[MONITOR INFO]===============", eh.ECHO_DEFAULT)
    for entry in parsed_desktopmonitorinfo:
        monitor_xml = ET.SubElement(monitorlist_xml, "MonitorDevice")
        ET.SubElement(monitor_xml, "Vendor").text =  str(entry['MonitorManufacturer'])
        ET.SubElement(monitor_xml, "Name").text =  str(entry['Name'])
        ET.SubElement(monitor_xml, "DevID").text =  str(entry['DeviceID'])
        ET.SubElement(monitor_xml, "ResolutionX").text =  str(entry['ScreenWidth'])
        ET.SubElement(monitor_xml, "ResolutionY").text =  str(entry['ScreenHeight'])
        eh.ui.Echo("DeviceID:\t" + str(entry['DeviceID']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("MonitorManufacturer: " + str(entry['MonitorManufacturer']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Height:\t\t" + str(entry['ScreenHeight']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Width:\t\t" + str(entry['ScreenWidth']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    eh.ui.Echo("===============[NETWORK ADAPTERS]===============", eh.ECHO_DEFAULT)
    for entry in parsed_adapterinfo:
        adapter_xml = ET.SubElement(netadapterlist_xml, "NetworkAdapter")
        ET.SubElement(adapter_xml, "Service").text =  str(entry['ServiceName'])
        ET.SubElement(adapter_xml, "Name").text =  str(entry['Name'])
        ET.SubElement(adapter_xml, "MAC").text =  str(entry['MACAddress'])
        ET.SubElement(adapter_xml, "Type").text =  str(entry['AdapterType'])
        ET.SubElement(adapter_xml, "DevID").text =  str(entry['DeviceID'])
        eh.ui.Echo("ServiceName:\t" + str(entry['ServiceName']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("MACAddress:\t" + str(entry['MACAddress']), eh.ECHO_DEFAULT)
        eh.ui.Echo("AdapterType:\t" + str(entry['AdapterType']), eh.ECHO_DEFAULT)
        eh.ui.Echo("DeviceID:\t" + str(entry['DeviceID']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    eh.ui.Echo("===============[SOUND DEVICES]===============", eh.ECHO_DEFAULT)
    for entry in parsed_sounddeviceinfo:
        sound_xml = ET.SubElement(soundlist_xml, "SoundDevice")
        ET.SubElement(sound_xml, "Vendor").text =  str(entry['Manufacturer'])
        ET.SubElement(sound_xml, "Name").text =  str(entry['Name'])
        eh.ui.Echo("Manufacturer:\t" + str(entry['Manufacturer']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    eh.ui.Echo("===============[PRINTER DEVICES]===============", eh.ECHO_DEFAULT)
    for entry in parsed_printerinfo:
        printer_xml = ET.SubElement(printerlist_xml, "NetworkAdapter")
        ET.SubElement(printer_xml, "SystemName").text =  str(entry['SystemName'])
        ET.SubElement(printer_xml, "Name").text =  str(entry['Name'])
        eh.ui.Echo("Name:\t\t" + str(entry['Name']), eh.ECHO_DEFAULT)
        eh.ui.Echo("SystemName:\t" + str(entry['SystemName']), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    api.WmiRelease(wmi_id)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")
