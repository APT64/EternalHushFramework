package eternalhush;

import console.ConsoleManager;
import console.OperationConsole;
import gui.IconLoader;
import gui.SettingsWindow;
import gui.TabPanel;
import gui.WindowConstants;

import javax.swing.*;

import java.awt.*;

public class EternalHushWindow extends JPanel {

    private IconLoader iconLoader = new IconLoader();
    public EternalHushWindow(final int type){
        setLayout(new BorderLayout());
        setType(type);
    }
    public EternalHushWindow(final int type, LayoutManager layout){
        setLayout(layout);
    }

    private void setType(final int type){

        switch (type) {
            case WindowConstants.MAIN_WINDOW:
                TabPanel operationPanel = new TabPanel();
                for (int i = 0; i < 3; i++){
                    OperationConsole consoleInstance = new OperationConsole();
                    operationPanel.addTab(consoleInstance.getHostname()+ " ["+consoleInstance.getConsoleId()+"]", iconLoader.loadIcon("images/console_icon.png", 31, 31), consoleInstance);
                }
                add(operationPanel);
                break;

            case WindowConstants.SETTINGS_WINDOW:
                SettingsWindow settings = new SettingsWindow();
                add(settings, BorderLayout.CENTER);
                break;

            default:
                break;
        }
    }
}
