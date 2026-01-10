package eternalhush.plugin.logviewer;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;

import eternalhush.console.OperationConsole;
import eternalhush.events.ConsoleEventListener;
import eternalhush.events.ConsoleEventSource;
import eternalhush.events.StatusEventListener;
import eternalhush.events.StatusEventSource;
import eternalhush.gui.TabPanel;
import eternalhush.console.ConsoleManager;


public class LogViewerTabPanel extends JPanel {
    TabPanel tabPanel;
    private void addConsoleLogViewer(OperationConsole console){
        JPanel p = new JPanel(new BorderLayout());
        TabPanel log_tab_panel = new TabPanel();
        log_tab_panel.addTab("Command Log",  new CommandLogViewerPanel(console));
        log_tab_panel.addTab("Invalid Log",  new InvalidCommandLogViewerPanel(console));
        p.add( log_tab_panel);
        tabPanel.addTab(console.getHostname()+ " [" +console.getConsoleId()+"]", p);
    }
    public LogViewerTabPanel(){
        super(new BorderLayout());
        tabPanel = new TabPanel();
        tabPanel.setTabPlacement(JTabbedPane.LEFT);

        ConsoleEventSource.addEventListener(new ConsoleEventListener() {
            @Override
            public void onConsoleTextChanged(OperationConsole console) {

            }

            @Override
            public void onStatusUpdated(String s) {

            }

            @Override
            public void onAllInstancesReady() {

            }

            @Override
            public void onInstanceReady(OperationConsole console) {
                addConsoleLogViewer(console);
            }

            @Override
            public void onCommandFinished(OperationConsole console) {

            }

            @Override
            public void onSessionLocked(OperationConsole operationConsole) {

            }
        });
        this.add(tabPanel, BorderLayout.CENTER);
    }

}
