package userinterface;

import console.CommandHandler;
import console.ConsoleManager;
import console.OperationConsole;

import java.util.Scanner;

public class UserInterface {
    public void echo(String str, int type) {
        OperationConsole consoleInstance = ConsoleManager.getInstance("localhost");

        String msgText = str;
        int msgType = type;

        if (msgType == MessagesConstants.ECHO_DEFAULT){
            consoleInstance.printDefault(msgText + "\n");
        }
        else if (msgType == MessagesConstants.ECHO_GOOD){
            consoleInstance.printSuccess(msgText + "\n");
        }
        else if (msgType ==  MessagesConstants.ECHO_ERROR){
            consoleInstance.printError(msgText + "\n");
        }
        else if (msgType ==  MessagesConstants.ECHO_WARNING){
            consoleInstance.printWarning(msgText + "\n");
        }
    }

    public String dialog(String str) {

        OperationConsole consoleInstance = ConsoleManager.getInstance("localhost");
        String msgText = str;
        consoleInstance.printDefault(msgText + "\n");

        String inputOption = null;
        Scanner scanner = consoleInstance.getDefaultScanner();
        if (scanner.hasNextLine()){
            inputOption = scanner.nextLine();
            consoleInstance.printDefault(inputOption + "\n");
        }

        return inputOption;
    }

    public int option(String str) {
        int option;

        OperationConsole consoleInstance = ConsoleManager.getInstance("localhost");
        String msgText = str;

        Scanner scanner = consoleInstance.getDefaultScanner();
        String inputOption = null;

        while (true){
            consoleInstance.printDefault(msgText + "\n");

            if (scanner.hasNextLine()){
                inputOption = scanner.nextLine();
                consoleInstance.printDefault(inputOption + "\n");
            }

            if (inputOption.equalsIgnoreCase(MessagesConstants.OPTION_CANCEL)){
                option = -1;
                break;
            }
            else if (inputOption.equalsIgnoreCase(MessagesConstants.OPTION_YES)){
                option = 1;
                break;
            }
            else if (inputOption.equalsIgnoreCase(MessagesConstants.OPTION_NO)){
                option = 0;
                break;
            }
            consoleInstance.printError("Unknown option \"" + inputOption + "\". Please, type \"Yes\", \"No\" or \"Cancel\"\n");
        }
        return option;
    }

    public void run(String str, int flag){

        OperationConsole consoleInstance = ConsoleManager.getInstance("localhost");

        String Command = str;
        int Option = flag;

        if (Option == MessagesConstants.RUN_SILENT){
            consoleInstance.outEnable(false);
        }
        CommandHandler handler = new CommandHandler();
        handler.CommonHandler(Command, consoleInstance);
        consoleInstance.outEnable(true);
    }

}
