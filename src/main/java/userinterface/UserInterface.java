package userinterface;

import console.CommandHandler;
import console.ConsoleManager;
import console.OperationConsole;

import java.awt.*;
import java.util.Scanner;

public class UserInterface {
    boolean lock = false;
    public void print_critical(String str) {
        OperationConsole consoleInstance = ConsoleManager.getInstance(0); //fix
        String msgText = str;
        consoleInstance.print(msgText, Color.RED);
    }

    public void echo(int consoleId, String str, int type) {
        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);

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

    public String dialog(int consoleId, String str, String input_type, String default_text) {

        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);
        String msgText = str;
        String defaultText = "";
        String inputOption;

        if (default_text != null){
            defaultText = "  (Default= " + default_text + ")";
        }
        consoleInstance.printDefault(msgText + defaultText +"\n");
        Scanner scanner = consoleInstance.getDefaultScanner();

        while (true) {
            if (scanner.hasNextLine()) {
                inputOption = scanner.nextLine();
                if (input_type.equals("INT") && default_text == null) {
                    try {
                        Integer.parseInt(inputOption);
                        break;
                    } catch (Exception e) {
                        consoleInstance.printError("Invalid int input\n");
                    }
                } else {
                    break;
                }
            }
        }
        if (inputOption.equals("")){
            inputOption = default_text;
        }
        consoleInstance.printDefault(inputOption + "\n");
        return inputOption;
    }

    public int option(int consoleId, String str, int default_type) {
        int option;

        String inputOption = "";
        String default_type_text;
        String defaultText;
        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);
        String msgText = str;

        if (default_type == MessagesConstants.OPTION_NO){
            defaultText = "NO";
        }else if (default_type == MessagesConstants.OPTION_YES){
            defaultText = "YES";
        }else {
            defaultText = "CANCEL";
        }

        default_type_text = "  (Default= " + defaultText + ")";

        Scanner scanner = consoleInstance.getDefaultScanner();

        consoleInstance.printDefault(msgText + default_type_text + "\n");
        while (true){
            if (scanner.hasNextLine()){
                inputOption = scanner.nextLine();
            }

            if (inputOption.equalsIgnoreCase("cancel")){
                option = MessagesConstants.OPTION_CANCEL;
                break;
            }
            else if (inputOption.equalsIgnoreCase("yes")){
                option = MessagesConstants.OPTION_YES;
                break;
            }
            else if (inputOption.equalsIgnoreCase("no")){
                option = MessagesConstants.OPTION_NO;
                break;
            }
            else if (inputOption.equals("")){
                option = default_type;
                inputOption = defaultText;
                break;
            }
            consoleInstance.printError("Unknown option \"" + inputOption + "\". Please, type \"Yes\", \"No\" or \"Cancel\"\n");
        }
        consoleInstance.printDefault(inputOption.toUpperCase() + "\n");
        return option;
    }

    public void run(int consoleId, String str, int flag){

        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);

        String Command = str;
        int Option = flag;

        if (Option == MessagesConstants.RUN_SILENT){
            consoleInstance.outEnable(false);
        }
        CommandHandler handler = new CommandHandler();
        handler.CommonHandler(Command, consoleInstance);
        consoleInstance.outEnable(true);
    }

    public void update_hostname(int consoleId, String hostname){
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        operationConsole.setHostname(hostname);
    }
}
