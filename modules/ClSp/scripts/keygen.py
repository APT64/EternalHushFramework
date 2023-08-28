import eternalhush as eh
import subprocess

WORKDIR = os.path.realpath(os.path.dirname(__file__)+"\\..")
FILEDIR = os.path.join(WORKDIR, "files")

def main(args):
    eh.ui.Echo("Generating RSA keypair", eh.ECHO_WARNING)
    keygen = os.path.join(FILEDIR, "X32_KeyGen.exe")
    subprocess.run([keygen, args[0]])
    eh.ui.Echo("Files written to", eh.ECHO_GOOD)
    eh.ui.Echo("   "+os.path.join(args[0], "private.key"), eh.ECHO_GOOD)
    eh.ui.Echo("   "+os.path.join(args[0], "public.key"), eh.ECHO_GOOD)
    
if __name__=="__main__":
    main(sys.argv)