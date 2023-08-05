package core;

import com.sun.jna.Library;
import com.sun.jna.Native;
import events.EternalEventSource;

public interface CoreConnector extends Library {
    CoreConnector Export = (CoreConnector)Native.load("EternalHushCore", CoreConnector.class);
    void RunScript(String script, int argc, String argv[]);
    void StartListener();
}
