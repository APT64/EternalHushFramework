import inspect
import eternalhush as eh
import _eternalhush


class TaskAborted(_eternalhush.ctxObj.ExceptionTaskAborted):
    def __init__(self, message):
        super().__init__(message)

class AttributeNotFound(Exception):
    def __init__(self, *args):
        self.msg = args[0]
      
    def __str__(self):
        return "Attribute {0} does not exist".format(self.msg)
        
def DeprecatedApiCall(apicall):
    def wrapper(*args, **kwargs):
        frame = inspect.currentframe()
        eh.ui.Echo("Warning: '{}' from {} is deprecated API!".format(apicall.__name__, frame.f_back.f_code.co_filename), eh.ECHO_WARNING)
        apicall(*args, **kwargs)
    return wrapper

def UnstableApiCall(apicall):
    def wrapper(*args, **kwargs):
        frame = inspect.currentframe()
        eh.ui.Echo("Warning: '{}'  is highly unstable API!".format(apicall.__name__), eh.ECHO_WARNING)
        ret_val = apicall(*args, **kwargs)
        return ret_val
    return wrapper

class InvalidApiCall(Exception): 
    def __init__(self, message): 
        super().__init__(message)