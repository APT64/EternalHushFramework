package manager;

import com.sun.jna.Native;
import console.OperationConsole;
import core.CoreConnector;
import eternalhush.GlobalVariables;
import org.apache.commons.lang3.ArrayUtils;

public class ExternalModules {
    private OperationConsole currentConsole;
    private String commandName;
    private CommonModule currentModule;

    public ExternalModules(OperationConsole console){
        currentConsole = console;
    }

    public void processCommand(String args[]) {
        commandName = args[0].substring(1);

        for (CommonModule module : GlobalVariables.commonModuleList){ //iterate loaded modules

            for (int j = 0; j < module.getCmdCount(); j++){ //iterate commands
                if (commandName.equalsIgnoreCase(module.getCmd(j).CommandName)){

                    if (args.length-1 != module.getCmd(j).getArgCount()){
                        currentConsole.printError("You provided " + (args.length-1) + " arguments, but only " + module.getCmd(j).getArgCount() +" were expected!\n");
                        return;
                    }
                    currentModule = module;
                    int id = j;
                    createTask(id, args);
                    return;
                }
            }
        }
        currentConsole.printError("Unrecognized external command \"" + commandName + "\"!\n");
        currentConsole.printError("Type \".help\" to display the available commands.\n");
    }

    private void createTask(int id, String args[]) {

        Commands currentCommand = currentModule.getCmd(id);
        args = ArrayUtils.remove(args, 0);

        String module =
                currentModule.WorkingDirectory
                + "\\scripts\\"
                + currentCommand.getProvider();

        Native.setProtected(true);
        try {
            CoreConnector.Export.RunScript(module, args.length, args);
        }catch (Error err){
            System.out.println(err);
        }
    }
}
