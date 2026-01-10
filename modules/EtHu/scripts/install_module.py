import eternalhush as eh
import subprocess
import os
import pip

def main(args):
    if not args.install and not args.remove and not args.list or args.install and args.remove and args.list:
        eh.ui.Echo("Failed to run installer: invalid arguments", eh.ECHO_ERROR)
        return
    path = "-t "+eh.WORKDIR+"\..\..\CoreLibs\Lib\site-packages"

    new_env = os.environ.copy()
    new_env["PYTHONHOME"] = eh.WORKDIR+"\..\..\CoreLibs"
    if args.install:
        proc = subprocess.Popen(eh.FILEDIR+"\\minpy.exe -m pip install "+args.install +" --disable-pip-version-check --root-user-action=ignore", env=new_env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    elif args.remove:
        proc = subprocess.Popen(eh.FILEDIR+"\\minpy.exe -m pip uninstall "+args.remove +" --yes --disable-pip-version-check --root-user-action=ignore", env=new_env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    elif args.list:
        proc = subprocess.Popen(eh.FILEDIR+"\\minpy.exe -m pip list --disable-pip-version-check", env=new_env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

    eh.ui.Echo("Starting installation process...", eh.ECHO_GOOD)
    for l in iter(proc.stdout.readline, b''):
        eh.ui.Echo(l.rstrip().decode(errors='ignore'), eh.ECHO_DEFAULT)
    eh.ui.Echo("Done!", eh.ECHO_GOOD)
    