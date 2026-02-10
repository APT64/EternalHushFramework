import eternalhush as eh

def select_layer_handler(parser):
    return parser.get_byte()

def finalize_layer_handler(parser):
    return parser.get_byte()

def read_mem_handler(parser):
    return parser.get_bstrarg()

def write_mem_handler(parser):
    return parser.get_int()

def leak_kernel_ob_handler(parser):
    return parser.get_long()