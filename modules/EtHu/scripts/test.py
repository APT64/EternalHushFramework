import eternalhush as eh
import os
    
def main(args):

    packet = eh.data.Struct({"val1": eh.LONG, "val2": eh.LONG, "val3": eh.LONG})
    packet.from_bytes(b'deadbeefdeadbeefdeadbeef')
    print(packet.data())
    
if __name__=="__main__":
    main(sys.argv)