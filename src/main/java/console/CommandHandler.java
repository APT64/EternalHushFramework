package console;

import manager.InternalCommands;
import manager.ExternalModules;


public class CommandHandler {
    InternalCommands internalCommands;

    public void CommonHandler(String line, OperationConsole console){
        internalCommands = new InternalCommands(console);

        if (getPrefix(line) == '!'){
            ExternalModules externalModules = new ExternalModules(console);
                String split_line[] = line.split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
                for (int i = 0; i < split_line.length; i++){
                    String str = split_line[i];
                    if (str.charAt(0) == '"' && str.charAt(str.length()-1) == '"'){
                        split_line[i] = str.substring(1, str.length()-1);
                    }
                }
                console.printWarning("[Started '" + split_line[0].substring(1) + "' [" + console.getHostname() + "]]\n" );
                externalModules.processCommand(split_line);
        }

        else if (getPrefix(line) == '.'){
            String split_line[] = line.split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
            for (int i = 0; i < split_line.length; i++){
                String str = split_line[i];
                if (str.charAt(0) == '"' && str.charAt(str.length()-1) == '"'){
                    split_line[i] = str.substring(1, str.length()-1);
                }
            }
            internalCommands.processCommand(split_line);
        }

        else {
            console.printError("Unrecognized command \"" + line + "\"!\n");
            console.printError("Type \".help\" to display the available commands.\n");
        }
    }
    public void CommonHandler(String line, String locked_module, OperationConsole console){
        internalCommands = new InternalCommands(console);

        if (getPrefix(line) == '!'){
            ExternalModules externalModules = new ExternalModules(console);
                String split_line[] = line.split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
                for (int i = 0; i < split_line.length; i++){
                    String str = split_line[i];
                    if (str.charAt(0) == '"' && str.charAt(str.length()-1) == '"'){
                        split_line[i] = str.substring(1, str.length()-1);
                    }
                }
                console.printWarning("[Started '" + split_line[0].substring(1) + "' [" + console.getHostname() + "]]\n" );
                externalModules.processCommand(split_line, locked_module);
        }
        else if (getPrefix(line) == '.'){
            String split_line[] = line.split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
            for (int i = 0; i < split_line.length; i++){
                String str = split_line[i];
                if (str.charAt(0) == '"' && str.charAt(str.length()-1) == '"'){
                    split_line[i] = str.substring(1, str.length()-1);
                }
            }
            internalCommands.processCommand(split_line, locked_module);
        }

        else {
            console.printError("Unrecognized command \"" + line + "\"!\n");
            console.printError("Type \".help\" to display the available commands.\n");
        }
    }
    private char getPrefix(String line){
        return  line.toCharArray()[0];
    }
}
