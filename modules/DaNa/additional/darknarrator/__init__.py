import eternalhush as eh
from additional.darknarrator import __ext_handlers as ext_handlers
DARKNARRATOR_UID = 0xc8ce81d43b1c32d6

READ_PHYS_MEM_UID = 0x64beb316e43e70cc
WRITE_PHYS_MEM_UID =0x62e45a28d884035c
WRITE_VIRT_MEM_UID =0xc25c2790bcfa8ab7
READ_VIRT_MEM_UID =0x8fe4455bdc4a21f9
SELECT_LAYER_UID =0xa5da5683c62f27ff
FINALIZE_LAYER_UID =0xe35496b5e2780cdf

LEAK_OBJECT_UID=0x121551f6baffd46b
LEAK_MODULE_UID= 0x12ea934ccc1d4060

THROTTLESTOP_LAYER =0x2b4e9984d442a315

DANA_SELECT_LAYER = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : SELECT_LAYER_UID,
    'api_args': [
        eh.LONGLONG
    ],
    
    'response_handler': ext_handlers.select_layer_handler
}

DANA_FINALIZE_LAYER = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : FINALIZE_LAYER_UID,
    'api_args': [
    ],
    
    'response_handler': ext_handlers.finalize_layer_handler
}

DANA_READ_VIRT_MEM = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : READ_VIRT_MEM_UID,
    'api_args': [
        eh.LONGLONG,
        eh.LONGLONG
    ],
    
    'response_handler': ext_handlers.read_mem_handler
}

DANA_READ_PHYS_MEM = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : READ_PHYS_MEM_UID,
    'api_args': [
        eh.LONGLONG,
        eh.LONGLONG
    ],
    
    'response_handler': ext_handlers.read_mem_handler
}

DANA_WRITE_VIRT_MEM = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : WRITE_VIRT_MEM_UID,
    'api_args': [
        eh.LONGLONG,
        eh.BYTES
    ],
    
    'response_handler': ext_handlers.write_mem_handler
}

DANA_WRITE_PHYS_MEM = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : WRITE_PHYS_MEM_UID,
    'api_args': [
        eh.LONGLONG,
        eh.BYTES
    ],
    
    'response_handler': ext_handlers.write_mem_handler
}

DANA_LEAK_KERNEL_OB = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : LEAK_OBJECT_UID,
    'api_args': [
        eh.LONGLONG,
        eh.LONGLONG
    ],
    
    'response_handler': ext_handlers.leak_kernel_ob_handler
}
DANA_LEAK_MODULE_ADDR = {
    'ext_uid' : DARKNARRATOR_UID,
    'api_uid' : LEAK_MODULE_UID,
    'api_args': [
        eh.STRING
    ],
    
    'response_handler': ext_handlers.leak_module_addr_handler
}



def DANA_IsReady():
    if eh.ui.GetEnv("DANA_REGISTERED") == 'true' and int(eh.ui.GetEnv("DANA_LAYER_UID")) > 16:
        return True
    return False

def DANA_IsInstalled():
    if eh.ui.GetEnv("DANA_REGISTERED") == 'true':
        return True
    return False

def DANA_GetLayer():
    return int(eh.ui.GetEnv("DANA_LAYER_UID"))