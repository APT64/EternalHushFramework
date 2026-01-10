package eternalhush.events;

import eternalhush.console.OperationConsole;

import java.util.ArrayList;

public class ConsoleEventSource {
    private static ArrayList<ConsoleEventListener> listenerList = new ArrayList<>();

    public static void addEventListener(ConsoleEventListener listener) {
        listenerList.add(listener);
    }

    public static void removeEventListener(ConsoleEventListener listener) {
        listenerList.remove(listener);
    }

    static public void consoleTextChanged(OperationConsole console) {
        for (ConsoleEventListener listener : listenerList) {
            listener.onConsoleTextChanged(console);
        }
    }

    static public void statusUpdated(String text) {
        for (ConsoleEventListener listener : listenerList) {
            listener.onStatusUpdated(text);
        }
    }

    static public void allInstancesReady() {
        for (ConsoleEventListener listener : listenerList) {
            listener.onAllInstancesReady();
        }
    }

    static public void instanceReady(OperationConsole console) {
        for (int i = 0; i < listenerList.size(); i++) {
            ConsoleEventListener listener = listenerList.get(i);
            listener.onInstanceReady(console);
        }
    }

    static public void commandFinished(OperationConsole console) {
        for (int i = 0; i < listenerList.size(); i++) {
            ConsoleEventListener listener = listenerList.get(i);
            listener.onCommandFinished(console);
        }
    }

    static public void sessionLocked(OperationConsole console) {
        for (int i = 0; i < listenerList.size(); i++) {
            ConsoleEventListener listener = listenerList.get(i);
            listener.onSessionLocked(console);
        }
    }
}
