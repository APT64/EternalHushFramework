from additional.clingyspider import __cmd_builder as cmd_builder
from additional.clingyspider import __response_parser as response_parser
from additional.clingyspider import __tasking as tasking
from additional.clingyspider.warnings import ClingySpider_API
from additional.clingyspider.structs import *
from additional.clingyspider.const import *
import eternalhush as eh


@ClingySpider_API
def RegisterUserExtension(extension_base):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(67)
    builder.add_long(extension_base)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_long()

@ClingySpider_API
def UnregisterUserExtension(extension_uid):
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(73)
    builder.add_long(extension_uid)
    data = tasking.call(builder.build())
    if data:
        parser.load_data(data)
        return parser.get_byte()

@ClingySpider_API
def EnumUserExtensions():
    parser = response_parser.ResponseParser()
    data = tasking.call_no_arg(72)
    result = []
    if data:
        parser.load_data(data)
        ext_count = parser.get_int()
        for i in range(ext_count):
            ext_uid = parser.get_long()
            ext_base = parser.get_long()
            result.append({'ext_uid':ext_uid, 'ext_base':ext_base})
        return result

@ClingySpider_API
def InvokeUserExtensionApi(ext_api_scheme, *varargs):
    varargs = tuple(varargs)
    parser = response_parser.ResponseParser()
    builder = cmd_builder.CommandBuilder()
    builder.set_command(78)
    builder.add_long(ext_api_scheme['ext_uid'])
    builder.add_long(ext_api_scheme['api_uid'])

    va_args = ext_api_scheme['api_args']
    if len(varargs) != len(va_args):
        eh.ui.Echo(f"InvokeUserExtensionApi: mismatch in the number of arguments! ({len(varargs)} out of {len(va_args)})", eh.ECHO_ERROR)
        return

    for arg_s, arg_v in zip(va_args, varargs):
        a_type = arg_s
        a_value = arg_v

        if a_type == eh.INT or a_type == eh.LONG or a_type == eh.LONGLONG or a_type == eh.CHAR or a_type == eh.SHORT:
            builder.add_long(a_value)
        elif a_type == eh.WSTRING:
            builder.add_wstrarg(a_value)
        elif a_type == eh.STRING:
            builder.add_strarg(a_value)
        elif a_type == eh.BYTES:
            builder.add_bstrarg(a_value)
        else:
            eh.ui.Echo(f"InvokeUserExtensionApi: unknown argument type! {str(a_type)}", eh.ECHO_ERROR)
            return
 

    data = tasking.call(builder.build())

    if data:
        parser.load_data(data)
        ret_object = ext_api_scheme['response_handler'](parser)
        lasterror = parser.get_long()
        eh.ui.SetLastError(lasterror)
        return ret_object