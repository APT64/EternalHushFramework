package eternalhush.plugin.remotefileviewer;
import eternalhush.console.CommandHandler;
import eternalhush.console.ConsoleManager;
import eternalhush.console.OperationConsole;
import eternalhush.events.ConsoleEventListener;
import eternalhush.events.ConsoleEventSource;
import eternalhush.gui.TabPanel;
import eternalhush.main.GlobalVariables;
import eternalhush.gui.IconLoader;
import eternalhush.gui.NativeFileDialog;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.table.AbstractTableModel;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

public class RFWTabPanel extends JPanel {

    TabPanel tabPanel;
    private void addConsoleLogViewer(OperationConsole console) {
        JPanel p = new JPanel(new BorderLayout());
        p.add(new RFWFileTable(console));
        tabPanel.addTab(console.getHostname() + " [" + console.getConsoleId() + "]", p);
    }
    private boolean isConsoleExists(OperationConsole console){
        for (int i =0; i < tabPanel.getTabCount(); i++){
            JPanel cp = ((JPanel)tabPanel.getComponentAt(i));
            for (Component component : cp.getComponents()) {
                if (component instanceof RFWFileTable) {
                    if (((RFWFileTable)component).linkedInstance == console) return true;
                }
            }
        }
        return false;
    }
    public RFWTabPanel(){
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
                    for(OperationConsole console : ConsoleManager.getInstanceList()) {
                        if (console.isSessionLocked() && console.getLockedModuleName().equalsIgnoreCase("clsp")) {
                            if(!isConsoleExists(console))
                                addConsoleLogViewer(console);
                        }
                    }
                }

                @Override
                public void onInstanceReady(OperationConsole console) {

                }

                @Override
                public void onCommandFinished(OperationConsole console) {

                }

                @Override
                public void onSessionLocked(OperationConsole console) {
                    if (console.getLockedModuleName().equalsIgnoreCase("clsp") && ConsoleManager.isInstanceReady(console.getConsoleId())) {
                        if(!isConsoleExists(console)) addConsoleLogViewer(console);
                    }
                }
            });
            this.add(tabPanel, BorderLayout.CENTER);
        }
    }

