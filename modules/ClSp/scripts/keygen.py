import eternalhush as eh
import os
import subprocess
import tempfile

def main(args):
    if args.algorithm.lower() == 'rsa2048':
        eh.ui.Echo("Generating RSA keypair", eh.ECHO_WARNING)
        keygen = os.path.join(eh.FILEDIR, "X32_KeyGen.exe")
        keytemp = tempfile.TemporaryDirectory()
        subprocess.run([keygen, keytemp.name])

        key1 = open(os.path.join(keytemp.name, "public.key"), "rb")
        key2 = open(os.path.join(keytemp.name, "private.key"), "rb")

        k1hex = key1.read().hex()
        k2hex = key2.read().hex()

        eh.ui.AddCryptoKey(k1hex, k2hex, args.name, eh.KEYTYPE_KEYPAIR, "RSA2048")

        key1.close()
        key2.close()
        keytemp.cleanup()
#        eh.ui.Echo("Files written to", eh.ECHO_GOOD)
#        eh.ui.Echo("   "+os.path.join(args.path, "private.key"), eh.ECHO_GOOD)
#        eh.ui.Echo("   "+os.path.join(args.path, "public.key"), eh.ECHO_GOOD)
    


