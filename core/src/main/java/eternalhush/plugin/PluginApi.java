package eternalhush.plugin;

import eternalhush.console.ConsoleManager;
import eternalhush.console.OperationConsole;
import eternalhush.events.ConsoleEventSource;

public abstract class PluginApi {
    public OperationConsole getConsoleById(int cid) {
        return ConsoleManager.getInstance(cid);
    }

    public OperationConsole getFirstFreeConsole() {
        return ConsoleManager.getFirstDefault();
    }

    public void printGlobalStatusText(String str) {
        ConsoleEventSource.statusUpdated(str);
    }
}
