package eternalhush.events;

import eternalhush.console.OperationConsole;

import java.util.EventListener;


public interface ConsoleEventListener extends EventListener {
    void onConsoleTextChanged(OperationConsole console);

    void onStatusUpdated(String text);

    void onAllInstancesReady();

    void onInstanceReady(OperationConsole console);

    void onCommandFinished(OperationConsole console);

    void onSessionLocked(OperationConsole console);
}
