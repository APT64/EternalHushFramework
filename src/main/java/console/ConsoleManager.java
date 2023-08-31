package console;

import java.util.ArrayList;

public abstract class ConsoleManager {
    private static ArrayList<OperationConsole> consoleInstances = new ArrayList<>();;

    public static void registerInstance(OperationConsole instance){
        consoleInstances.add(instance);
    }

    public static OperationConsole getInstance(int id){
        try {
            return consoleInstances.get(id);
        }catch (Exception e){
            return null;
        }
    }

    public static int getCount(){
        return consoleInstances.size();
    }
}
