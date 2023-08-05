package events;


import java.util.ArrayList;

public class EternalEventSource {
    private static ArrayList<EternalEventListener> listenerList = new ArrayList<>();

    public static void addEventListener(EternalEventListener listener) {
        listenerList.add(listener);
    }

    public static void removeEventListener(EternalEventListener listener) {
        listenerList.remove(listener);
    }

    static public void initCompleted() {
        for (EternalEventListener listener : listenerList){
            listener.onInitCompleted();
        }
    }
}
