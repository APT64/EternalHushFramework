import _eternalhush

def ImportRsaKey(path, type):
    return _eternalhush.ctxObj.import_rsa_key(path, type)

def DecryptRsaData(key, encrypted_data):
    return _eternalhush.ctxObj.decrypt_rsa_data(key, encrypted_data)

def EncryptRsaData(key, encrypted_data):
    return _eternalhush.ctxObj.decrypt_rsa_data(key, encrypted_data)
    
def DecryptAesData(key, iv, encrypted_data):
    if isinstance(key, str):
        key = bytearray.fromhex(key)
    if isinstance(iv, str):
        iv = bytearray.fromhex(iv)
    return _eternalhush.ctxObj.decrypt_aes_data(key, iv, encrypted_data)    
    
def EncryptAesData(key, iv, encrypted_data):
    if isinstance(key, str):
        key = bytearray.fromhex(key)
    if isinstance(iv, str):
        iv = bytearray.fromhex(iv)
    return _eternalhush.ctxObj.encrypt_aes_data(key, iv, encrypted_data)
    
def PJWHASH(key):
   BitsInUnsignedInt = 4 * 8
   ThreeQuarters     = int((BitsInUnsignedInt  * 3) / 4)
   OneEighth         = int(BitsInUnsignedInt / 8)
   HighBits          = (0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth)
   hash              = 0
   test              = 0

   for i in range(len(key)):
     hash = (hash << OneEighth) + key[i]
     test = hash & HighBits
     if test != 0:
       hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
   return (hash & 0x7FFFFFFF)
