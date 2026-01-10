package eternalhush.manager;

import org.apache.commons.lang3.ArrayUtils;
import eternalhush.console.OperationConsole;
import eternalhush.main.GlobalLogger;
import eternalhush.main.GlobalVariables;

import java.util.ArrayList;
import java.util.HashMap;

public class InternalCommands {
    private OperationConsole currentConsole;

    public InternalCommands(OperationConsole console) {
        currentConsole = console;
    }

    public String createHelp(Commands cmd) {
        if (cmd.getArgCount() > 0) {
            String base_help = "Usage: " + cmd.CommandName;
            base_help += " [Options]\n   " + cmd.CommandDescription + "\n\n";
            base_help += "Options:\n";
            for (Arguments argument : cmd.getArgList()) {
                String optional = "";
                if (argument.IsOptional.equalsIgnoreCase("true") || argument.IsOptional.equalsIgnoreCase("1")) {
                    optional = "[OPTIONAL]";
                }
                base_help += "   " + argument.ArgumentName + " [" + argument.ArgumentType + "] " + optional + "\n";
                base_help += "      " + argument.ArgumentDescription + "\n";
            }
            base_help += "\n";
            return base_help;
        } else {
            String base_help = "Usage: " + cmd.CommandName;
            base_help += " \n   " + cmd.CommandDescription + "\n\n";
            return base_help;
        }
    }

    private void logInvalidCommand(String cmd_name, int id) {
        GlobalVariables.GlobalOperCommandLogger.writeIoLog(2, null, null, 12, id, cmd_name, currentConsole.getConsoleId());
    }

    private void logValidCommand(String cmd_name, int id, int result) {
        GlobalVariables.GlobalOperCommandLogger.writeIoLog(1, null, 0, null, id, cmd_name, currentConsole.getConsoleId());
        GlobalVariables.GlobalOperCommandLogger.writeIoLog(2, null, 0, result, id, cmd_name, currentConsole.getConsoleId());
    }

    public void processCommand(String[] args, int command_id) {
        String commandName = args[0].substring(1);
        args = ArrayUtils.remove(args, 0);
        GlobalLogger.log(String.format("Dispatching internal command '%s'", commandName));
        switch (commandName) {
            case "clear":
                currentConsole.getOutputArea().setText(null);
                currentConsole.clearLogFile();
                currentConsole.printDefault("");
                logValidCommand(commandName, command_id, 0);
                return;
            case "help":
                if (args.length == 0) {
                    String buffer = "";
                    ArrayList<String> cmd_names = new ArrayList<>();
                    currentConsole.printDefault("Usage: help <command>\n\n");
                    currentConsole.printDefault("Available external commands:\n");
                    for (int i = 0; i < GlobalVariables.commonModuleList.size(); i++) {
                        CommonModule currentModule = GlobalVariables.commonModuleList.get(i);
                        for (int j = 0; j < currentModule.getCmdCount(); j++) {
                            if (currentModule.getCmd(j).Dependency.equals("default") || currentModule.getCmd(j).Dependency.equals("*")) {
                                cmd_names.add(currentModule.getCmd(j).CommandName);
                                if (cmd_names.size() == 3) {
                                    buffer += String.format(" %1$-25s %2$-25s %3$-25s\n", cmd_names.toArray());
                                    cmd_names.clear();
                                }
                            }
                        }
                    }
                    currentConsole.printDefault(buffer);
                    currentConsole.printDefault("\nAvailable internal commands:\n");
                    String[] inrnl = {"set", "clear", "echo", "warn", "error", "quit", "title", "", ""};
                    for (int i = 0; i < inrnl.length; i += 3) {
                        currentConsole.printDefault(String.format(" %1$-25s %2$-25s %3$-25s\n", inrnl[i], inrnl[i + 1], inrnl[i + 2]));
                    }
                    logValidCommand(commandName, command_id, 0);
                } else {
                    for (int i = 0; i < GlobalVariables.commonModuleList.size(); i++) {
                        CommonModule currentModule = GlobalVariables.commonModuleList.get(i);
                        for (int j = 0; j < currentModule.getCmdCount(); j++) {
                            if (currentModule.getCmdName(j).equals(args[0])) {
                                currentConsole.printDefault(createHelp(currentModule.getCmd(j)));
                                logValidCommand(commandName, command_id, 0);
                                return;
                            }
                        }
                    }
                    currentConsole.printError("Command \"" + args[0] + "\" does not exist\n");
                    logValidCommand(commandName, command_id, 10);
                }
                return;
            case "set":
                if (args.length == 0) {
                    for (Object entryObject : currentConsole.getEnv().entrySet()) {
                        HashMap.Entry<String, String> entry = (HashMap.Entry) entryObject;
                        String key = entry.getKey();
                        String value = entry.getValue();
                        currentConsole.printDefault(key + "=" + value + "\n");
                    }
                    logValidCommand(commandName, command_id, 0);
                    return;
                } else if (args.length > 2) {
                    currentConsole.printError("Too many arguments provided!\n");
                    logInvalidCommand(commandName, command_id);
                    return;
                } else if (args.length == 1) {
                    currentConsole.getEnv().get(args[0]);
                    if (currentConsole.getEnv().get(args[0]) != null) {
                        currentConsole.printDefault(args[0] + "=" + currentConsole.getEnv().get(args[0]).toString() + "\n");
                        logValidCommand(commandName, command_id, 0);
                    } else {
                        currentConsole.printError("Environment variable \"" + args[0] + "\" is not defined!\n");
                        logValidCommand(commandName, command_id, 10);
                    }
                    return;
                } else {
                    currentConsole.putEnvVar(args[0], args[1]);
                    logValidCommand(commandName, command_id, 0);
                }
                return;
            case "quit":
                currentConsole.printWarning("Exiting from application...\n");
                logValidCommand(commandName, command_id, 0);
                System.exit(0);
                return;

            case "echo":
                if (args.length != 1) {
                    currentConsole.printError("Invalid argument count\n");
                    logInvalidCommand(commandName, command_id);
                    return;
                }
                currentConsole.printDefault(args[0] + "\n");
                logValidCommand(commandName, command_id, 0);
                return;
            case "warn":
                if (args.length != 1) {
                    currentConsole.printError("Invalid argument count\n");
                    logInvalidCommand(commandName, command_id);
                    return;
                }
                currentConsole.printWarning(args[0] + "\n");
                logValidCommand(commandName, command_id, 0);
                return;
            case "error":
                if (args.length != 1) {
                    currentConsole.printError("Invalid argument count\n");
                    logInvalidCommand(commandName, command_id);
                    return;
                }
                currentConsole.printError(args[0] + "\n");
                logValidCommand(commandName, command_id, 0);
                return;
            case "title":
                if (args.length == 0) {
                    currentConsole.printDefault(GlobalVariables.rootFrame.getTitle() + "\n");
                    logValidCommand(commandName, command_id, 0);
                    return;
                } else if (args.length == 1) {
                    GlobalVariables.rootFrame.setTitle(args[0]);
                    logValidCommand(commandName, command_id, 0);
                    return;
                } else {
                    currentConsole.printError("Invalid argument count\n");
                    logInvalidCommand(commandName, command_id);
                    return;
                }
        }

        currentConsole.printError("Unrecognized internal command \"" + commandName + "\"!\n");
        currentConsole.printError("Type \".help\" to display the available commands.\n");
        logInvalidCommand(commandName, command_id);
    }

    public void processCommand(String[] args, String locked_module, int command_id) {
        String[] original_args = args;
        String commandName = args[0].substring(1);
        GlobalLogger.log(String.format("Dispatching internal command '%s' with lock %s", commandName, locked_module));
        args = ArrayUtils.remove(args, 0);
        switch (commandName) {
            case "help":
                if (args.length == 0) {
                    String buffer = "";
                    ArrayList<String> cmd_names = new ArrayList<>();
                    currentConsole.printDefault("Usage: help <command>\n\n");
                    currentConsole.printDefault("Available external commands:\n");
                    for (int i = 0; i < GlobalVariables.commonModuleList.size(); i++) {
                        CommonModule currentModule = GlobalVariables.commonModuleList.get(i);
                        for (int j = 0; j < currentModule.getCmdCount(); j++) {
                            if (currentModule.getCmd(j).Dependency.equals("default") || currentModule.getCmd(j).Dependency.equals("*") || currentModule.getCmd(j).Dependency.equalsIgnoreCase(locked_module)) {
                                cmd_names.add(currentModule.getCmd(j).CommandName);
                                if (cmd_names.size() == 3) {
                                    buffer += String.format(" %1$-25s %2$-25s %3$-25s\n", cmd_names.toArray());
                                    cmd_names.clear();
                                }
                            }
                        }
                    }
                    currentConsole.printDefault(buffer);
                    currentConsole.printDefault("\nAvailable internal commands:\n");
                    String[] inrnl = {"set", "clear", "echo", "warn", "error", "quit", "title", "", ""};
                    for (int i = 0; i < inrnl.length; i += 3) {
                        currentConsole.printDefault(String.format(" %1$-25s %2$-25s %3$-25s\n", inrnl[i], inrnl[i + 1], inrnl[i + 2]));
                    }
                    logValidCommand(commandName, command_id, 0);
                } else {
                    for (int i = 0; i < GlobalVariables.commonModuleList.size(); i++) {
                        CommonModule currentModule = GlobalVariables.commonModuleList.get(i);
                        for (int j = 0; j < currentModule.getCmdCount(); j++) {
                            if (currentModule.getCmdName(j).equals(args[0])) {
                                currentConsole.printDefault(createHelp(currentModule.getCmd(j)));
                                logValidCommand(commandName, command_id, 0);
                                return;
                            }
                        }
                    }
                    currentConsole.printError("Command \"" + args[0] + "\" does not exist\n");
                    logValidCommand(commandName, command_id, 10);
                }
                return;
            default:
                processCommand(original_args, command_id);
        }
    }
}
