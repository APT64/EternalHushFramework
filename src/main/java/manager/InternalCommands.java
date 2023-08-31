package manager;

import console.OperationConsole;
import core.CoreConnector;
import eternalhush.GlobalVariables;
import org.apache.commons.lang3.ArrayUtils;

public class InternalCommands {
    private OperationConsole currentConsole;

    public InternalCommands(OperationConsole console){
        currentConsole = console;
    }

    public String createHelp(Commands cmd) {
        if (cmd.getArgCount() > 0){
            String base_help = "Usage: " + cmd.CommandName;
            base_help += " [Options]\n   " + cmd.CommandDescription + "\n\n";
            base_help += "Options:\n";
            for (Arguments argument : cmd.getArgList()){
                base_help += "   " + argument.ArgumentName + " ["+ argument.ArgumentType +"]\n";
                base_help += "      " + argument.ArgumentDescription +"\n";
            }
            base_help += "\n";
            return base_help;
        }
        else {
            String base_help = "Usage: " + cmd.CommandName;
            base_help += " \n   " + cmd.CommandDescription + "\n\n";
            return base_help;
        }
    }

    public void processCommand(String[] args){
        String commandName = args[0].substring(1);
        args = ArrayUtils.remove(args, 0);
        switch (commandName) {

            case "clear":
                currentConsole.getOutputArea().setText(null);
                return;
            case "help":
                if (args.length == 0){
                    currentConsole.printDefault("Usage: help <command>\n\n");
                    currentConsole.printDefault("Available external commands:\n");
                    for (int i = 0; i < GlobalVariables.commonModuleList.size(); i++){
                        CommonModule currentModule = GlobalVariables.commonModuleList.get(i);
                        for (int j = 0; j < currentModule.getCmdCount(); j++) {
                            currentConsole.printDefault(currentModule.getCmdName(j) + "\n");
                        }
                    }
                    currentConsole.printDefault("\nAvailable internal commands:\n");
                    currentConsole.printDefault("help\nclear\npython\nquit\n");
                }
                else {
                    for (int i = 0; i < GlobalVariables.commonModuleList.size(); i++){
                        CommonModule currentModule = GlobalVariables.commonModuleList.get(i);
                        for (int j = 0; j < currentModule.getCmdCount(); j++) {
                            if (currentModule.getCmdName(j).equals(args[0])){
                                currentConsole.printDefault(createHelp(currentModule.getCmd(j)));
                                return;
                            }
                        }
                    }
                    currentConsole.printError("Command \"" + args[0] + "\" does not exist\n");
                }
                return;
            case "python":
                if (args.length == 0){
                    currentConsole.printError("No arguments provided!\n");
                    return;
                }
                String module = args[0];
                args = ArrayUtils.remove(args, 0);
                CoreConnector.Export.RunScript(module, currentConsole.getConsoleId(), args.length, args);
                return;
            case "quit":
                currentConsole.printWarning("Exiting from application...\n");
                System.exit(0);
                return;
        }

        currentConsole.printError("Unrecognized internal command \"" + commandName + "\"!\n");
        currentConsole.printError("Type \".help\" to display the available commands.\n");
    }
}
