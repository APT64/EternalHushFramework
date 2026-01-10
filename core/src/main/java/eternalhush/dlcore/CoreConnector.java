package eternalhush.dlcore;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface CoreConnector extends Library {
    CoreConnector Export = Native.load("EternalHushCore", CoreConnector.class);

    int RunScript(String script, int console_id, long flags, int arglen, Pointer argv, String uid);

    void TerminateRunningScript(String uid);

    String GetTaskScriptName(String uid);

    int GetTaskCid(String uid);

    int GetTaskFlags(String uid);
}
