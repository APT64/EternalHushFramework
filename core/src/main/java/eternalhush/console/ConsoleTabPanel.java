package eternalhush.console;

import eternalhush.events.ConsoleEventListener;
import eternalhush.events.ConsoleEventSource;
import eternalhush.gui.IconLoader;
import eternalhush.gui.TabPanel;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;

public class ConsoleTabPanel extends TabPanel {
    int reservedTabCount = 2;
    public ConsoleTabPanel() {
        super();
        this.addTab(null, new IconLoader().loadIcon("images/add.png", 30, 30), new JPanel(), "Create new instance");
        this.addChangeListener(new ChangeListener() {
            public void stateChanged(ChangeEvent evt) {
                JTabbedPane tabbedPane = (JTabbedPane) evt.getSource();
                if (getToolTipTextAt(tabbedPane.getSelectedIndex()) != null) {
                    OperationConsole consoleInstance = new OperationConsole();
                    addTab(consoleInstance.getHostname() + " [" + consoleInstance.getConsoleId() + "]", new IconLoader().loadIcon("images/tv.png", 32, 32), consoleInstance);
                    setSelectedIndex(consoleInstance.getConsoleTabId());
                    consoleInstance.setCmdStatus_Idle();
                } else {
                    int idx = getSelectedIndex();
                    String old_text = getTitleAt(idx);
                    if (old_text.startsWith("* ")) {
                        setTitleAt(idx, old_text.substring(2));
                    }
                }
            }
        });
        ConsoleEventSource.addEventListener(new ConsoleEventListener() {
            @Override
            public void onConsoleTextChanged(OperationConsole console) {
                if (console.getDetached()) return;
                if (ConsoleTabPanel.this.getSelectedIndex() != console.getConsoleTabId()) {
                    String old_text = ConsoleTabPanel.this.getTitleAt(console.getConsoleTabId());
                    if (!old_text.startsWith("* "))
                        ConsoleTabPanel.this.setTitleAt(console.getConsoleTabId(), "* " + old_text);
                }
            }

            @Override
            public void onStatusUpdated(String text) {

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
    }

    @Override
    public void addTab(String string, Icon icon, Component comp) {
        super.addTab(string, icon, comp);
        int totalTabs = this.getTabCount();
        for (int i = 0; i < totalTabs; i++) {
            if (getToolTipTextAt(i) != null) {
                removeTabAt(i);
                this.addTab(null, new IconLoader().loadIcon("images/add.png", 30, 30), new JPanel(), "Create new instance");
                break;
            }
        }
    }
}
