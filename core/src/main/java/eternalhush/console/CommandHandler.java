package eternalhush.console;

import eternalhush.events.ConsoleEventSource;
import eternalhush.main.GlobalLogger;
import eternalhush.main.GlobalVariables;
import eternalhush.manager.ExternalModules;
import eternalhush.manager.InternalCommands;


public class CommandHandler {
    InternalCommands internalCommands;

    public void CommonHandler(String line, OperationConsole console, int primary_run) {
        GlobalLogger.log(String.format("Dispatching command '%s'", line));
        int _id = console.getLastCommandId();
        console.setLastCommandId(_id + 1);
        internalCommands = new InternalCommands(console);
        GlobalVariables.GlobalOperCommandLogger.writeIoLog(0, line, null, null, _id, null, console.getConsoleId());
        line = line.replace("\\", "\\\\");
        String[] split_line = line.split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
        if (getPrefix(line) == '!') {
            ExternalModules externalModules = new ExternalModules(console);
            for (int i = 0; i < split_line.length; i++) {
                String str = split_line[i];
                if (str.charAt(0) == '"' && str.charAt(str.length() - 1) == '"') {
                    split_line[i] = str.substring(1, str.length() - 1);
                }
            }
            externalModules.processCommand(split_line, _id, primary_run);
        } else if (getPrefix(line) == '.') {
            for (int i = 0; i < split_line.length; i++) {
                String str = split_line[i];
                if (str.charAt(0) == '"' && str.charAt(str.length() - 1) == '"') {
                    split_line[i] = str.substring(1, str.length() - 1);
                }
            }
            internalCommands.processCommand(split_line, _id);
        } else {
            console.printError("Unrecognized command \"" + line + "\"!\n");
            console.printError("Type \".help\" to display the available commands.\n");
            GlobalVariables.GlobalOperCommandLogger.writeIoLog(2, null, null, 12, _id, split_line[0], console.getConsoleId());
        }
        ConsoleEventSource.commandFinished(console);
    }

    public void CommonHandler(String line, String locked_module, OperationConsole console, int primary_run) {
        GlobalLogger.log(String.format("Dispatching command '%s'", line));
        int _id = console.getLastCommandId();
        console.setLastCommandId(_id + 1);
        internalCommands = new InternalCommands(console);
        GlobalVariables.GlobalOperCommandLogger.writeIoLog(0, line, null, null, _id, null, console.getConsoleId());
        line = line.replace("\\", "\\\\");
        String[] split_line = line.split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
        if (getPrefix(line) == '!') {
            ExternalModules externalModules = new ExternalModules(console);
            for (int i = 0; i < split_line.length; i++) {
                String str = split_line[i];
                if (str.charAt(0) == '"' && str.charAt(str.length() - 1) == '"') {
                    split_line[i] = str.substring(1, str.length() - 1);
                }
            }
            externalModules.processCommand(split_line, locked_module, _id, primary_run);
        } else if (getPrefix(line) == '.') {
            for (int i = 0; i < split_line.length; i++) {
                String str = split_line[i];
                if (str.charAt(0) == '"' && str.charAt(str.length() - 1) == '"') {
                    split_line[i] = str.substring(1, str.length() - 1);
                }
            }
            internalCommands.processCommand(split_line, locked_module, _id);
        } else {
            console.printError("Unrecognized command \"" + line + "\"!\n");
            console.printError("Type \".help\" to display the available commands.\n");
            GlobalVariables.GlobalOperCommandLogger.writeIoLog(2, null, null, 12, _id, split_line[0], console.getConsoleId());
        }
        ConsoleEventSource.commandFinished(console);
    }

    private char getPrefix(String line) {
        return line.toCharArray()[0];
    }
}
