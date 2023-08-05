package manager;

import console.OperationConsole;
import core.CoreConnector;
import eternalhush.GlobalVariables;
import org.apache.commons.lang3.ArrayUtils;

public class InternalCommands {
    private OperationConsole currentConsole;
    private String commandName;

    public InternalCommands(OperationConsole console){
        currentConsole = console;
    }

    public void processCommand(String[] args){
        commandName = args[0].substring(1);
        args = ArrayUtils.remove(args, 0);
        switch (commandName) {

            case "clear":
                currentConsole.getOutputArea().setText(null);
                return;
            case "help":
                currentConsole.printDefault("Available external commands:\n");
                for (int i = 0; i < GlobalVariables.commonModuleList.size(); i++){
                    CommonModule currentModule = GlobalVariables.commonModuleList.get(i);
                    for (int j = 0; j < currentModule.getCmdCount(); j++) {
                        currentConsole.printDefault(currentModule.getCmdName(j) + "\n");
                    }
                }
                currentConsole.printDefault("\nAvailable internal commands:\n");
                currentConsole.printDefault("help\nclear\npython\nquit\n");
                return;
            case "python":
                String module = args[0];
                args = ArrayUtils.remove(args, 0);
                CoreConnector.Export.RunScript(module, args.length, args);
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
