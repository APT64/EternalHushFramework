import eternalhush as eh
import _eternalhush
from additional.clingyspider import __cmd_builder as cmd_builder
from additional.clingyspider import __response_parser as response_parser
from additional.clingyspider import __tasking as tasking
from eternalhush.exception import DeprecatedApiCall, UnstableApiCall
from additional.clingyspider.warnings import ClingySpider_API
from additional.clingyspider.structs import *
from additional.clingyspider.const import *
import random

def CSSysCreateWorkerContext():
    try:
        if _eternalhush.ctxObj.worker_ctx_id == 0:
            #create
            workerctxid = random.randint(1, 10000)
            _eternalhush.ctxObj.worker_ctx_id = workerctxid
            
    except:
        workerctxid = random.randint(1, 10000)
        _eternalhush.ctxObj.worker_ctx_id = workerctxid
        #create
    return _eternalhush.ctxObj.worker_ctx_id

def CSSysGetWorkerContext():
    return _eternalhush.ctxObj.worker_ctx_id

def CSSysDestoryWorkerContext():
    _eternalhush.ctxObj.worker_ctx_id = 0