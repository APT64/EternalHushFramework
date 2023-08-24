import eternalhush
import os

def main(args):
    resp = eternalhush.ui.Option("Antivirus detected! Do you want to run?")
    if resp == eternalhush.OPTION_YES:
        os.system(args[0])
    else:
        eternalhush.ui.Echo("Aborting...", eternalhush.ECHO_WARNING)
        
if __name__ == "__main__":
    main(sys.argv)