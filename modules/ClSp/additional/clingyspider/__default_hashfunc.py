def default_hashfunc(key):
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
