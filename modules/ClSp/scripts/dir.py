import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
import os
import xml.etree.cElementTree as ET
import xml.dom.minidom


def fix_path(path):
    path = path.replace("\\", "\\\\")
    path = path.replace("/", "\\\\")
    if path[-1:] != "\\" and path[-1:] != "/":
        path+="\\\\"
    return path

def cvt_wmi_datetime(dt):
    base_dt = dt.split(".")[0]
    year = base_dt[0:4]
    month = base_dt[4:6]
    day = base_dt[6:8]
    hour = base_dt[8:10]
    minute = base_dt[10:12]
    second = base_dt[12:14]
    return "{}.{}.{} {}:{}:{}".format(day, month, year, hour, minute, second)

def save_xml_output(data, path):
    root = ET.Element("DirectoryList")
    for e in data:
        fname = e[0]
        faccess = e[1]
        fsystem = e[2]
        fhidden = e[3]
        fsize = e[4]
        ftimestamp = e[5]
        if fsize == "<directory>":
            fsize = '0'
            ftype = 'directory'
        else:
            ftype = 'file'
        
        ET.SubElement(root, "FileEntry", type=ftype, access=faccess, hidden=fhidden, system=fsystem, size=fsize, timestamp=ftimestamp).text = fname
    from datetime import datetime
    root.set('timestamp', datetime.now().isoformat())
    root.set('taskuid', eh.TASK_UID)
    tree = ET.ElementTree(root)
    ET.indent(tree, level=0)
    tree.write(path, encoding='utf-8', xml_declaration=True)

def main(args):
    if args.path:
        dir_path = args.path
    else:
        dir_path = api.GetVirtualWorkdir()

    drive_letter = dir_path[:2]
    if drive_letter[1] != ":":
        eh.ui.Echo("Invalid path specified", eh.ECHO_ERROR)
        return
    path = dir_path[2:]
    path = fix_path(path)

    wmi_id = api.WmiInitialize()
    file_flt_hdr = ["Name", "Access", "System", "Hidden", "Size", "Modified"]
    files_data = []
    if wmi_id == None:
        eh.ui.Echo("Failed to initialize wmi session", eh.ECHO_ERROR)
        return
    if not api.WmiConnect(wmi_id, "root\\CIMV2"):
        eh.ui.Echo("Failed to connect to wmi resource 'root\\CIMV2'", eh.ECHO_ERROR)
        return
    if not api.WmiQuery(wmi_id, 'SELECT * FROM Win32_Directory WHERE Drive = "'+drive_letter+'" AND Path = "'+path+'"'):
        eh.ui.Echo("Failed to query Win32_Directory class", eh.ECHO_ERROR)
        return
    parsed = api.WmiParseResult(wmi_id, ["Name", "Readable", "System", "Hidden", "FileSize", "LastModified"])
    for row in parsed:
        dir_entry = []
        for _entry in row.items():
            if _entry[0] == "Readable":
                if _entry[1]:
                    dir_entry.append("r")
                else:
                    dir_entry.append("-")
            elif _entry[0] == "System":
                if _entry[1]:
                    dir_entry.append("true")
                else:
                    dir_entry.append("false")
            elif _entry[0] == "Hidden":
                if _entry[1]:
                    dir_entry.append("true")
                else:
                    dir_entry.append("false")
            elif _entry[0] == "LastModified":
                dir_entry.append(cvt_wmi_datetime(_entry[1]))
            elif _entry[0] == "FileSize":
                dir_entry.append("<directory>")
            else:
                dir_entry.append(_entry[1])
        files_data.append(dir_entry)

    if not api.WmiQuery(wmi_id, 'SELECT * FROM CIM_DataFile WHERE Drive = "'+drive_letter+'" AND Path = "'+path+'"'):
        eh.ui.Echo("Failed to query CIM_DataFile class", eh.ECHO_ERROR)
        return
    parsed = api.WmiParseResult(wmi_id, ["Name", "Readable", "Writeable", "System", "Hidden", "FileSize", "LastModified"])
    for row in parsed:
        dir_entry = []
        for _entry in row.items():
            if _entry[0] == "Readable":
                if _entry[1]:
                    dir_entry.append("r")
            elif _entry[0] == "Writeable":
                if _entry[1]:
                    previous_val = dir_entry.pop()
                    if previous_val != "" and previous_val != "r":
                        dir_entry.append(previous_val)
                        continue
                    previous_val+= "w"
                    dir_entry.append(previous_val)
            elif _entry[0] == "System":
                if _entry[1]:
                    dir_entry.append("true")
                else:
                    dir_entry.append("false")
            elif _entry[0] == "Hidden":
                if _entry[1]:
                    dir_entry.append("true")
                else:
                    dir_entry.append("false")
            elif _entry[0] == "LastModified":
                dir_entry.append(cvt_wmi_datetime(_entry[1]))
            else:
                dir_entry.append(_entry[1])
        files_data.append(dir_entry)
    
    if args.xmlout:
        save_xml_output(files_data, args.xmlout)

    formatted_filelist = eh.ui.FormatTable(file_flt_hdr, files_data, max_elem_sz = 10)
    eh.ui.Echo(str(formatted_filelist), eh.ECHO_DEFAULT)
    api.WmiRelease(wmi_id)
