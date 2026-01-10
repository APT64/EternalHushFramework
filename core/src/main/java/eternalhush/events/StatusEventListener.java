package eternalhush.events;

import java.util.EventListener;

public interface StatusEventListener extends EventListener {
    void onInitCompleted();

    void onPostInitCompleted();
}