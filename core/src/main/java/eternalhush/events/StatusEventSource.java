package eternalhush.events;


import eternalhush.main.GlobalVariables;

import java.util.ArrayList;

public class StatusEventSource {
    private static ArrayList<StatusEventListener> listenerList = new ArrayList<>();

    public static void addEventListener(StatusEventListener listener) {
        listenerList.add(listener);
    }

    public static void removeEventListener(StatusEventListener listener) {
        listenerList.remove(listener);
    }

    static public void initCompleted() {
        GlobalVariables.OperationConf.initDone = true;
        for (StatusEventListener listener : listenerList) {
            listener.onInitCompleted();
        }
    }

    static public void postInitCompleted() {
        GlobalVariables.OperationConf.postInitDone = true;
        for (StatusEventListener listener : listenerList) {
            listener.onPostInitCompleted();
        }
    }
}
