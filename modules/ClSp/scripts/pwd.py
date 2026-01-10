import eternalhush as eh
from additional import clingyspider, silentnight
import os


def main(args):
    print(hex(clingyspider.VirtualAlloc(1000, clingyspider.MEM_COMMIT | clingyspider.MEM_RESERVE, clingyspider.PAGE_READWRITE)))

if __name__ == "__main__":
    parser = eh.data.ArgParser()
    main(parser.parse())