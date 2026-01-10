import _eternalhush
import os
import eternalhush.ui
import eternalhush.net
import eternalhush.data
import eternalhush.utils
import eternalhush.exception
import eternalhush.crypto


WORKDIR = _eternalhush.ctxObj.workdir
STORAGEDIR = _eternalhush.ctxObj.storage
FILEDIR = _eternalhush.ctxObj.files

FORCE = _eternalhush.ctxObj.force_flag
CONSOLE_ID = _eternalhush.ctxObj.console_id
TASK_UID = _eternalhush.ctxObj.task_uid

STRING = _eternalhush.ctxObj.STRING
INT = _eternalhush.ctxObj.INT
ECHO_DEFAULT = _eternalhush.ctxObj.ECHO_DEFAULT
ECHO_GOOD = _eternalhush.ctxObj.ECHO_GOOD
ECHO_ERROR = _eternalhush.ctxObj.ECHO_ERROR
ECHO_WARNING = _eternalhush.ctxObj.ECHO_WARNING
ECHO_NOENDL = _eternalhush.ctxObj.ECHO_NOENDL
OPTION_YES = _eternalhush.ctxObj.OPTION_YES
OPTION_NO = _eternalhush.ctxObj.OPTION_NO
OPTION_CANCEL = _eternalhush.ctxObj.OPTION_CANCEL
RUN_SILENT = _eternalhush.ctxObj.RUN_SILENT
RUN_NORMAL = _eternalhush.ctxObj.RUN_NORMAL

TCP_CONNECTION = _eternalhush.ctxObj.TCP_CONNECTION
HTTP_CONNECTION = _eternalhush.ctxObj.HTTP_CONNECTION

LONGLONG = _eternalhush.ctxObj.LONGLONG
LONG = _eternalhush.ctxObj.LONG
SHORT = _eternalhush.ctxObj.SHORT
CHAR = _eternalhush.ctxObj.CHAR

PRIVATE_KEY = _eternalhush.ctxObj.PRIVATE_KEY
PUBLIC_KEY = _eternalhush.ctxObj.PUBLIC_KEY

KEYTYPE_KEYPAIR = _eternalhush.ctxObj.KEYTYPE_KEYPAIR