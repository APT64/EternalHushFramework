package eternalhush.console;

import eternalhush.events.ConsoleEventListener;
import eternalhush.events.ConsoleEventSource;
import eternalhush.gui.IconLoader;

import javax.swing.*;
import java.awt.*;

public class StatusPanel extends JPanel {
    IconLoader iconLoader;
    private JTextField logTextOutput;


    public StatusPanel() {
        iconLoader = new IconLoader();
        logTextOutput = new JTextField();
        logTextOutput.setEditable(false);
        logTextOutput.setFocusable(false);

        setLayout(new BorderLayout());
        ConsoleEventSource.addEventListener(new ConsoleEventListener() {
            @Override
            public void onConsoleTextChanged(OperationConsole console) {

            }

            @Override
            public void onStatusUpdated(String text) {
                logTextOutput.setText(text);
            }

            @Override
            public void onAllInstancesReady() {

            }

            @Override
            public void onInstanceReady(OperationConsole console) {

            }

            @Override
            public void onCommandFinished(OperationConsole console) {

            }

            @Override
            public void onSessionLocked(OperationConsole console) {

            }
        });
        add(logTextOutput, BorderLayout.CENTER);
    }
}
