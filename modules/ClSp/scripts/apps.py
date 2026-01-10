import eternalhush as eh
from additional.clingyspider import const
from additional.clingyspider import structs
from additional.clingyspider import api
import sqlite3
import os
import xml.etree.cElementTree as ET
import xml.dom.minidom

def query_software_key(hkey, path):
    handle = api.RegOpenKey(hkey, path, const.KEY_READ)
    if not handle:
        eh.ui.Echo("query_software_key(): failed to open handle", eh.ECHO_ERROR)
        return
    
    keylist = api.RegEnumKey(handle)
    ret_list = []
    for key in keylist:
        key_handle = api.RegOpenKey(hkey, path+"\\"+key, const.KEY_READ)
        eh.ui.SuppressEcho(eh.ECHO_ERROR)
        value = api.RegQueryValue(key_handle, "DisplayName")
        eh.ui.UnsuppressEcho(eh.ECHO_ERROR)
        if value != None:
            name = value.data.get().decode('utf-16-le').strip('\x00')
            eh.ui.SuppressEcho(eh.ECHO_ERROR)
            location = api.RegQueryValue(key_handle, "InstallLocation")
            eh.ui.UnsuppressEcho(eh.ECHO_ERROR)
            if location:
                location = location.data.get().decode('utf-16-le').strip('\x00')
            else:
                location = ""
            ret_list.append({'name': name, 'location': location, 'key':key})
        api.CloseHandle(key_handle)
    api.CloseHandle(handle)

    return ret_list

def main(args):
    eh.ui.Echo("Collecting information about X64 installed software", eh.ECHO_DEFAULT)
    software64 = query_software_key(const.HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
    if software64:
        eh.ui.Echo('Done', eh.ECHO_GOOD)
    else:
        eh.ui.Echo('Done with errors', eh.ECHO_WARNING)
    eh.ui.Echo("Collecting information about Wow64 installed software", eh.ECHO_DEFAULT)
    software6432 = query_software_key(const.HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
    if software6432:
        eh.ui.Echo('Done', eh.ECHO_GOOD)
    else:
        eh.ui.Echo('Done with errors', eh.ECHO_WARNING)
    eh.ui.Echo("Collecting information about X64 user-installed software", eh.ECHO_DEFAULT)
    usersoftware64 = query_software_key(const.HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
    if usersoftware64:
        eh.ui.Echo('Done', eh.ECHO_GOOD)
    else:
        eh.ui.Echo('Done with errors', eh.ECHO_WARNING)
    eh.ui.Echo("Collecting information about Wow64 user-installed software", eh.ECHO_DEFAULT)
    usersoftware6432 = query_software_key(const.HKEY_CURRENT_USER, "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
    if usersoftware6432:
        eh.ui.Echo('Done', eh.ECHO_GOOD)
    else:
        eh.ui.Echo('Done with errors', eh.ECHO_WARNING)

    xmlroot = ET.Element("SoftwareList")
    eh.ui.Echo("============== Local Computer Software List: =============", eh.ECHO_DEFAULT)
    for entry in software64:
        soft64_xml = ET.SubElement(xmlroot, "Installation")
        ET.SubElement(soft64_xml, "Name").text =  entry['name']
        ET.SubElement(soft64_xml, "Location").text =  entry['location']
        ET.SubElement(soft64_xml, "Key").text =  entry['key']
        ET.SubElement(soft64_xml, "Scope").text = 'systemwide'

        eh.ui.Echo("Name:\t\t" + entry['name'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Key:\t\t" + entry['key'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Location:\t" + entry['location'], eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    eh.ui.Echo("============== Local Computer Wow64 Software List: ======", eh.ECHO_DEFAULT)

    for entry in software6432:
        soft64_xml = ET.SubElement(xmlroot, "Installation")
        ET.SubElement(soft64_xml, "Name").text =  entry['name']
        ET.SubElement(soft64_xml, "Location").text =  entry['location']
        ET.SubElement(soft64_xml, "Key").text =  entry['key']
        ET.SubElement(soft64_xml, "Scope").text = 'systemwide_wow64'

        eh.ui.Echo("Name:\t\t" + entry['name'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Key:\t\t" + entry['key'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Location:\t" + entry['location'], eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    eh.ui.Echo("============== User Software List: =====================", eh.ECHO_DEFAULT)

    for entry in usersoftware64:
        soft64_xml = ET.SubElement(xmlroot, "Installation")
        ET.SubElement(soft64_xml, "Name").text =  entry['name']
        ET.SubElement(soft64_xml, "Location").text =  entry['location']
        ET.SubElement(soft64_xml, "Key").text =  entry['key']
        ET.SubElement(soft64_xml, "Scope").text = 'userwide'

        eh.ui.Echo("Name:\t\t" + entry['name'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Key:\t\t" + entry['key'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Location:\t" + entry['location'], eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)
        
    eh.ui.Echo("============== User Wow64 Software List: ==============", eh.ECHO_DEFAULT)
    for entry in usersoftware6432:
        soft64_xml = ET.SubElement(xmlroot, "Installation")
        ET.SubElement(soft64_xml, "Name").text =  entry['name']
        ET.SubElement(soft64_xml, "Location").text =  entry['location']
        ET.SubElement(soft64_xml, "Key").text =  entry['key']
        ET.SubElement(soft64_xml, "Scope").text = 'userwide_wow64'

        eh.ui.Echo("Name:\t\t" + entry['name'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Key:\t\t" + entry['key'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Location:\t" + entry['location'], eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")