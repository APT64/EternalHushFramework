import eternalhush as eh
import datetime
from additional.clingyspider import api
from additional.clingyspider import const
import xml.etree.cElementTree as ET
import xml.dom.minidom

def cvt_wmi_datetime(dt):
    base_dt = dt.split(".")[0]
    year = base_dt[0:4]
    month = base_dt[4:6]
    day = base_dt[6:8]
    hour = base_dt[8:10]
    minute = base_dt[10:12]
    second = base_dt[12:14]
    return [day, month, year, hour, minute, second]

def main(args):
    xmlroot = ET.Element("TargetSystemTime")
    
    ptr = api.LocalAlloc(8)
    status = api.UserSyscall("NtQuerySystemTime", ptr)
    if status == 0:
        timebytes = api.ReadMemory(ptr, 8)
        ms = int.from_bytes(timebytes, 'little')
        dtime = eh.utils.SystimeToDatetime(ms)
        eh.ui.Echo("System Time: {:02d}.{:02d}.{:02d} {:02d}:{:02d}:{:02d}".format(dtime[2], dtime[1], dtime[0], dtime[3], dtime[4], dtime[5]), eh.ECHO_DEFAULT)

    api.LocalFree(ptr)

    wmi_id = api.WmiInitialize()
    file_flt_hdr = ["LocalDateTime"]
    files_data = []
    if wmi_id == None:
        eh.ui.Echo("Failed to initialize wmi session", eh.ECHO_ERROR)
        return
    if not api.WmiConnect(wmi_id, "root\\CIMV2"):
        eh.ui.Echo("Failed to connect to wmi resource 'root\\CIMV2'", eh.ECHO_ERROR)
        return
    if not api.WmiQuery(wmi_id, 'SELECT LocalDateTime FROM Win32_OperatingSystem'):
        eh.ui.Echo("Failed to query Win32_OperatingSystem class", eh.ECHO_ERROR)
        return
    parsed = api.WmiParseResult(wmi_id, file_flt_hdr)[0]['LocalDateTime']
    tp = cvt_wmi_datetime(parsed)
    eh.ui.Echo("Local Time: " + "{}.{}.{} {}:{}:{}".format(tp[0], tp[1], tp[2], tp[3], tp[4], tp[5]), eh.ECHO_DEFAULT)

    localtime_xml = ET.SubElement(xmlroot, "LocalTime")
    ET.SubElement(localtime_xml, "Day").text =  tp[0]
    ET.SubElement(localtime_xml, "Month").text =  tp[1]
    ET.SubElement(localtime_xml, "Year").text =  tp[2]
    ET.SubElement(localtime_xml, "Hour").text =  tp[3]
    ET.SubElement(localtime_xml, "Minute").text =  tp[4]
    ET.SubElement(localtime_xml, "Second").text =  tp[5]

    if not api.WmiQuery(wmi_id, 'SELECT Caption, Bias FROM Win32_TimeZone'):
        eh.ui.Echo("Failed to query Win32_TimeZone class", eh.ECHO_ERROR)
        return
    dtz = api.WmiParseResult(wmi_id, ['Caption', 'Bias'])[0]
    tz = dtz['Caption']
    bias = dtz['Bias']
    
    localtz_xml = ET.SubElement(xmlroot, "LocalTimeZone")
    ET.SubElement(localtz_xml, "Caption").text = str(tz)
    ET.SubElement(localtz_xml, "Bias").text = str(bias)

    eh.ui.Echo("Timezone: " + tz, eh.ECHO_DEFAULT)

    if args.xmlout:
        from datetime import datetime
        xmlroot.set('timestamp', datetime.now().isoformat())
        xmlroot.set('taskuid', eh.TASK_UID)
        tree = ET.ElementTree(xmlroot)
        ET.indent(tree, level=0)
        tree.write(args.xmlout, xml_declaration=True, encoding="utf-8")