import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const


def main(args):


    wmi_id = api.WmiInitialize()
    proc_data = []
    if wmi_id == None:
        eh.ui.Echo("Failed to initialize wmi session", eh.ECHO_ERROR)
        return
    if not api.WmiConnect(wmi_id, "root\\CIMV2"):
        eh.ui.Echo("Failed to connect to wmi resource 'root\\CIMV2'", eh.ECHO_ERROR)
        return
    if not api.WmiQuery(wmi_id, "SELECT * FROM Win32_Process"):
        eh.ui.Echo("Failed to query Win32_Process class", eh.ECHO_ERROR)
        return
    if not args.minimal:
        eh.ui.Echo("Dumping all information, this may take up to a few minutes...", eh.ECHO_WARNING)
        parsed_proclist = api.WmiParseResult(wmi_id, ["Name", "ProcessId", "ParentProcessId", "CommandLine"])
    else:
        parsed_proclist = api.WmiParseResult(wmi_id, ["Name", "ProcessId", "ParentProcessId"])
    
    if not args.minimal:
        if not api.WmiQuery(wmi_id, "SELECT * FROM Win32_Process"):
            eh.ui.Echo("Failed to query Win32_Process class", eh.ECHO_ERROR)
            return
        api.WmiExecMethod(wmi_id, "Win32_Process", "GetOwner")
        parsed_procuser = api.WmiParseMethodResult(wmi_id, ["User", "Domain"])
    api.WmiRelease(wmi_id)

    cnt = 0
    for row in parsed_proclist:
        proc_entry = []
        for _entry in row.values():
            proc_entry.append(_entry)
        if not args.minimal:
            userdomain = str(parsed_procuser[cnt]['Domain']) +"\\"+str(parsed_procuser[cnt]['User'])
            proc_entry.append(userdomain)
        else:
            proc_entry.append("")
            proc_entry.append("")
        proc_data.append(proc_entry)
        cnt += 1


#    if not args.minimala:
#        cnt = 0
#        for entry in proc_data:
#            eh.ui.SuppressEcho(eh.ECHO_ERROR)

#            pid = entry[1]
#            handle = api.GetProcessHandle(pid, const.PROCESS_QUERY_INFORMATION)
#            if not handle:
#                uid = ""
#            else:
#                htoken = api.GetProcessToken(handle, const.TOKEN_QUERY)
#                uid = api.TokenGetUID(htoken)
#                api.CloseHandle(htoken)
#                api.CloseHandle(handle)
#            proc_data[cnt].append(uid)
#            cnt+=1
#            eh.ui.UnsuppressEcho(eh.ECHO_ERROR)

    #formatted_proclist = eh.ui.FormatTable(proc_flt_hdr, proc_data, max_elem_sz=10)
    for e in proc_data:
        eh.ui.Echo("-----------------------------------------------------", eh.ECHO_DEFAULT)
        eh.ui.Echo(f"Name    : {str(e[0])}", eh.ECHO_DEFAULT)
        eh.ui.Echo(f"PID     : {str(e[1])}", eh.ECHO_DEFAULT)
        eh.ui.Echo(f"PPID    : {str(e[2])}", eh.ECHO_DEFAULT)
        eh.ui.Echo(f"CmdLine : {str(e[3])}", eh.ECHO_DEFAULT)
        eh.ui.Echo(f"Owner   : {str(e[4])}", eh.ECHO_DEFAULT)
