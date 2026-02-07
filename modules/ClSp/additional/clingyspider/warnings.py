from eternalhush.exception import InvalidApiCall
from eternalhush.ui import GetLockedModule
from additional.clingyspider import __systemapi
from functools import wraps

def ClingySpider_API_wrapper(singlethread):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            lock_module = GetLockedModule()
            if lock_module.lower() != "clsp":
                raise InvalidApiCall("Cannot call this API outside the module!")
            if singlethread:
                ctxworkerid = __systemapi.CSSysCreateWorkerContext()
            return_value = func(*args, **kwargs)
            return return_value
        return wrapper
    return decorator

def ClingySpider_API(call = None, *, singlethread=False):
    wrap_decorator = ClingySpider_API_wrapper(singlethread)
    if call is None:
        return wrap_decorator
    else:
        return wrap_decorator(call)