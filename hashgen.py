hashes = []

def check_collision():
  seen = set()
  duplicates = []

  # Iterate over each element in the list
  for i in hashes:
      if i in seen:
          duplicates.append(i)
      else:
          seen.add(i)
  print(duplicates)

def PJWHash(key):
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
   hashes.append(hex((hash & 0x7FFFFFFF)))
   return hex((hash & 0x7FFFFFFF))

def hhash(line):
  pline = line
  if pline.endswith('.dll') or pline.endswith('.exe'):
     pline = pline[:-4]
  print("#define HASH_{} {}".format(pline, PJWHash(line.encode())))
  print("#define WHASH_{} {}".format(pline, PJWHash(line.encode('utf-16-le'))))


hhash("ntdll.dll")
hhash("RemoteCall")
hhash("kernel32.dll")
hhash("ieframe.dll")
hhash("IECreateFile")
hhash("BaseThreadInitThunk")
hhash("RtlUserThreadStart")
hhash("winhttp.dll")
hhash("WinHttpOpenRequest")
hhash("WinHttpCloseHandle")
hhash("WinHttpWriteData")
hhash("WinHttpReceiveResponse")
hhash("WinHttpSendRequest")
hhash("WinHttpSetTimeouts")
hhash("WinHttpReadData")
hhash("WinHttpQueryDataAvailable")
hhash("WinHttpQueryHeaders")
hhash("WinHttpSetOption")

hhash("WinHttpOpen")
hhash("WinHttpConnect")

hhash("RtlAllocateHeap")
hhash("RtlFreeHeap")
hhash("RtlCreateProcessParametersEx")
hhash("RtlProcessHeap")
hhash("RtlDestroyProcessParameters")
hhash("CreateEnvironmentBlock")

hhash("CreateToolhelp32Snapshot")
hhash("Thread32First")
hhash("Thread32Next")
hhash("kernelbase.dll")

hhash("gdiplus.dll")
hhash("GdipCreateBitmapFromHBITMAP")
hhash("GdipSaveImageToStream")
hhash("GdipDisposeImage")
hhash("GdiplusStartup")
hhash("GdiplusShutdown")

hhash("WinHttpAddRequestHeaders")

hhash("RegisterExt")

check_collision()