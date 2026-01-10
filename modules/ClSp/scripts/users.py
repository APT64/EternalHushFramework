import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import time
import xml.etree.cElementTree as ET
import xml.dom.minidom

def main(args):
    wmi_id = api.WmiInitialize()
    if wmi_id == None:
        eh.ui.Echo("Failed to initialize wmi session", eh.ECHO_ERROR)
        return
    if not api.WmiConnect(wmi_id, "root\\CIMV2"):
        eh.ui.Echo("Failed to connect to wmi resource 'root\\CIMV2'", eh.ECHO_ERROR)
        return
    if not api.WmiQuery(wmi_id, "SELECT * FROM Win32_UserAccount"):
        eh.ui.Echo("Failed to query Win32_UserAccount class", eh.ECHO_ERROR)
        return

    xmlroot = ET.Element("TargetUsers")
    parsed = api.WmiParseResult(wmi_id, ["AccountType", "Caption", "Domain", "SID", "Name", "Disabled", "Description"])
    for entry in parsed:
        userlist_xml = ET.SubElement(xmlroot, "UserAccount")
        ET.SubElement(userlist_xml, "AccountType").text =  str(entry['AccountType'])
        ET.SubElement(userlist_xml, "Caption").text =  str(entry['Caption'])
        ET.SubElement(userlist_xml, "Domain").text =  str(entry['Domain'])
        ET.SubElement(userlist_xml, "SID").text =  str(entry['SID'])
        ET.SubElement(userlist_xml, "Name").text =  str(entry['Name'])
        account_desc = str(entry['Description'])
        if len(account_desc) < 2:
            account_desc = ""
        ET.SubElement(userlist_xml, "Description").text =  account_desc
        ET.SubElement(userlist_xml, "Enabled").text = str(bool(not entry['Disabled']))
        eh.ui.Echo("Name        :\t" + entry['Name'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Description :\t" + account_desc, eh.ECHO_DEFAULT)
        eh.ui.Echo("AccountType :\t" + str(entry['AccountType']), eh.ECHO_DEFAULT)
        eh.ui.Echo("Caption     :\t" + entry['Caption'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Domain      :\t" + entry['Domain'], eh.ECHO_DEFAULT)
        eh.ui.Echo("SID         :\t" + entry['SID'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Enabled     :\t" + str(bool(not entry['Disabled'])), eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    api.WmiRelease(wmi_id)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")

