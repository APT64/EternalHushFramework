package eternalhush;

import console.ConsoleManager;
import console.OperationConsole;
import core.CoreConnector;
import events.EternalEventSource;
import gui.BasicFrame;
import gui.IconLoader;
import gui.TabPanel;
import gui.WindowConstants;

import manager.ModuleLoader;
import userinterface.GateWay;

import javax.swing.plaf.FontUIResource;
import java.awt.*;
import java.io.IOException;

public class EternalHushMain {

    protected EternalHushWindow operationWindow;
    protected EternalHushWindow settingsWindow;
    protected IconLoader iconLoader;
    protected ModuleLoader moduleLoader;

    public EternalHushMain(String args[], BasicFrame frame) throws IOException, InterruptedException {
        /*  Инициализируем глобальные перемнные  */
        GlobalVariables.rootFrame = frame;

        new GateWay().start(Integer.parseInt(SettingsLoader.getKeyValue("py4j_port")));

        iconLoader = new IconLoader();
        /*  Загружаем скрипты и плагины  */

        moduleLoader = new ModuleLoader();
        moduleLoader.load(GlobalVariables.commonModuleList);

        /*  Настройка и GUI  */
        frame.setTitle("EternalHush Framework " + GlobalVariables.VERSION);

        TabPanel tabPanel = new TabPanel();

        operationWindow = new EternalHushWindow(WindowConstants.MAIN_WINDOW);
        settingsWindow = new EternalHushWindow(WindowConstants.SETTINGS_WINDOW);

        tabPanel.addTab("Operation", iconLoader.loadIcon("images/console_icon.png"), operationWindow);
        tabPanel.addTab("Settings", iconLoader.loadIcon("images/settings_icon.png"), settingsWindow);

        frame.add(tabPanel);
        frame.setSize(GlobalVariables.screenSize.width / 2, GlobalVariables.screenSize.height / 2);
        frame.setLocation(GlobalVariables.screenSize.width / 4, GlobalVariables.screenSize.height / 4);

        frame.pack();
        frame.setVisible(true);

        postInit();
    }

    public static void main(String args[]) throws IOException, InterruptedException {
        BasicFrame.setupLookAndFeel(SettingsLoader.getKeyValue("default_theme"));
        BasicFrame.setGuiFont(new FontUIResource(SettingsLoader.getKeyValue("gui_font"), Font.PLAIN, Integer.parseInt(SettingsLoader.getKeyValue("gui_font_size"))));

        BasicFrame mainframe = new BasicFrame();
        new EternalHushMain(args, mainframe);

    }

    private void postInit() throws IOException, InterruptedException{
        EternalEventSource.initCompleted();
    }
}
