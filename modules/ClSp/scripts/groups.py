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
    if not api.WmiQuery(wmi_id, "SELECT * FROM Win32_Group"):
        eh.ui.Echo("Failed to query Win32_Group class", eh.ECHO_ERROR)
        return

    xmlroot = ET.Element("TargetGroups")
    parsed = api.WmiParseResult(wmi_id, ["Caption", "Domain", "SID", "Name", "Description"])
    for entry in parsed:
        grouplist_xml = ET.SubElement(xmlroot, "UserGroup")
        ET.SubElement(grouplist_xml, "Caption").text =  str(entry['Caption'])
        ET.SubElement(grouplist_xml, "Domain").text =  str(entry['Domain'])
        ET.SubElement(grouplist_xml, "SID").text =  str(entry['SID'])
        ET.SubElement(grouplist_xml, "Name").text =  str(entry['Name'])
        account_desc = str(entry['Description'])
        if len(account_desc) < 2:
            account_desc = ""
        ET.SubElement(grouplist_xml, "Description").text =  account_desc
        eh.ui.Echo("Name        :\t" + entry['Name'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Description :\t" + account_desc, eh.ECHO_DEFAULT)
        eh.ui.Echo("Caption     :\t" + entry['Caption'], eh.ECHO_DEFAULT)
        eh.ui.Echo("Domain      :\t" + entry['Domain'], eh.ECHO_DEFAULT)
        eh.ui.Echo("SID         :\t" + entry['SID'], eh.ECHO_DEFAULT)
        eh.ui.Echo("", eh.ECHO_DEFAULT)

    api.WmiRelease(wmi_id)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")