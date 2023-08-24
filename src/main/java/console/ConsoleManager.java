package console;

import java.util.ArrayList;

public abstract class ConsoleManager {
    private static ArrayList<OperationConsole> consoleInstances = new ArrayList<>();;

    public static void registerInstance(OperationConsole instance){
        consoleInstances.add(instance);
    }

    public static OperationConsole getInstance(String target){
        for(OperationConsole console : consoleInstances){
            if (console.getTarget().equals(target)){
               return console;
            }
        }
        return null;
    }
}
