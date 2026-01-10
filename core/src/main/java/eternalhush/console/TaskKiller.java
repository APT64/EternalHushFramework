package eternalhush.console;

import eternalhush.dlcore.CoreConnector;
import eternalhush.userinterface.MessagesConstants;
import eternalhush.userinterface.UserInterface;

import java.io.IOException;

public class TaskKiller {
    public TaskKiller(OperationConsole console) throws IOException {
        console.setCmdStatus_Running();
        if (!console.forceabortexecution) {
            console.forceabortexecution = true;
            String query = "Do you want to abort current running task?\n";

            int opt = new UserInterface().option(console.getConsoleId(), null, query, MessagesConstants.OPTION_NO);
            console.forceabortexecution = false;
            if (opt == MessagesConstants.OPTION_YES) {
                initiateKilling(console);
            }
            console.clrQueryText();
            console.setCmdStatus_Idle();
        } else {
            console.emulateUserInput("YES");
        }
    }

    void initiateKilling(OperationConsole console) {
        CoreConnector.Export.TerminateRunningScript(console.getCurrentTaskUID());
        console.setCurrentTaskUID("");
    }
}
