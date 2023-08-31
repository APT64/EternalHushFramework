package core;

import com.sun.jna.Library;
import com.sun.jna.Native;

public interface CoreConnector extends Library {
    CoreConnector Export = (CoreConnector)Native.load("EternalHushCore", CoreConnector.class);
    void RunScript(String script, int console_id, int argc, String argv[]);
}
