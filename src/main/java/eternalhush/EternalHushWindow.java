package eternalhush;

import console.OperationConsole;
import gui.SettingsWindow;
import gui.WindowConstants;

import javax.swing.*;

import java.awt.*;

public class EternalHushWindow extends JPanel {

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
                OperationConsole operationConsole = new OperationConsole();
                add(operationConsole);
                break;

            case WindowConstants.SETTINGS_WINDOW:
                SettingsWindow settings = new SettingsWindow();
                add(settings, BorderLayout.CENTER);
                break;

            case WindowConstants.EMPTY_WINDOW:
                break;

            default:
                break;
        }
    }
}
