import eternalhush as eh
from additional.clingyspider import const
from additional.clingyspider import api
from additional.clingyspider import structs
import sqlite3
import os

def ClingyspiderDefaultCallback(cid):
    if eh.ui.GetEnv('MIBA_SURVEY_RUN', console_id= cid).lower() == 'true':
        eh.ui.Run('!survey -profile initialize', eh.RUN_NORMAL, console_id= cid)
        known = eh.ui.GetEnv('MIBA_SURVEY_KNOWN_TARGET', console_id= cid)
        if known.lower() == 'true':
            rescan = eh.ui.GetEnv("MIBA_SURVEY_RESCAN_TARGET")
            if rescan.lower() == 'true':
                eh.ui.Echo(f"MIBA_SURVEY_RESCAN_TARGET set to 'true', starting survey script", eh.ECHO_DEFAULT, console_id= cid)
                eh.ui.Run('!survey -profile informational', eh.RUN_NORMAL, console_id= cid)
            elif rescan.lower() == 'false':
                eh.ui.Echo(f"MIBA_SURVEY_RESCAN_TARGET set to 'false', skipping survey script", eh.ECHO_DEFAULT, console_id= cid)
            elif rescan.lower() == 'ask':
                opt = eh.ui.Option("Target already exists in database. Do you want to rescan and save it?", default=eh.OPTION_NO, console_id= cid)
                if opt == eh.OPTION_YES:
                    eh.ui.Run('!survey -profile informational', eh.RUN_NORMAL, console_id= cid)
        elif known.lower() == 'false':
            eh.ui.Echo(f"Starting scan of new target system", eh.ECHO_DEFAULT, console_id= cid)
            eh.ui.Run('!survey -profile informational', eh.RUN_NORMAL, console_id= cid)