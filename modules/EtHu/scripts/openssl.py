import eternalhush as eh
import os
import subprocess

def main(args):

    proc = subprocess.Popen(eh.FILEDIR+"\\openssl\\bin\\openssl.exe " + args.args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    for l in iter(proc.stdout.readline, b''):
        eh.ui.Echo(l.rstrip().decode(errors='ignore'), eh.ECHO_DEFAULT)

    eh.ui.Echo("Process finished.", eh.ECHO_GOOD)
    

