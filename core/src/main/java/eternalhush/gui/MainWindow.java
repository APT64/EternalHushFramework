package eternalhush.gui;

import eternalhush.console.CommandLogger;
import eternalhush.dlcore.PluginClassLoader;
import eternalhush.events.StatusEventSource;
import eternalhush.main.*;
import eternalhush.manager.ModuleLoader;
import eternalhush.plugin.BasePluginInterface;
import eternalhush.userinterface.GateWay;

import javax.swing.*;
import java.io.File;
import java.io.IOException;
import java.util.Objects;

public class MainWindow extends JWindow {

    protected EternalHushWindow operationWindow;
    protected EternalHushWindow settingsWindow;
    protected IconLoader iconLoader;
    protected ModuleLoader moduleLoader;
    protected PluginClassLoader classLoader;

    public MainWindow(String[] args, BasicFrame frame) throws IOException, InterruptedException {
        try {
            TabPanel tabPanel = new TabPanel();
            GlobalVariables.GlobalOperCommandLogger = new CommandLogger();
            GlobalVariables.rootFrame = frame;
            GlobalVariables.rootTabPanel = tabPanel;
            GlobalLogger.log("Starting py4j gateway");
            GlobalVariables.loadScreen.updateStatus("Setting up user-scripting gateway", 0);
            int gws_port = Integer.parseInt(SettingsLoader.getKeyValue("py4j_port"));
            GlobalLogger.log("Binding port " + gws_port);
            new GateWay().start(gws_port);

            iconLoader = new IconLoader();
            GlobalLogger.log("Loading .JAR plugins");
            GlobalVariables.loadScreen.updateStatus("Loading .JAR plugins", 5);
            classLoader = new PluginClassLoader();
            File curdir = new File("plugins");
            for (File fileEntry : Objects.requireNonNull(curdir.listFiles())) {
                BasePluginInterface pluginInterface = classLoader.load("plugins/" + fileEntry.getName());
                GlobalLogger.log("Loading plugin: " + pluginInterface.getPluginName());
                GlobalVariables.pluginList.add(pluginInterface);
            }

            moduleLoader = new ModuleLoader();
            GlobalLogger.log("Module loader started");
            GlobalVariables.loadScreen.updateStatus("Loading modules", 10);
            moduleLoader.load(GlobalVariables.commonModuleList, GlobalVariables.metaModuleList);

            frame.setTitle("EternalHush Framework " + GlobalVariables.VERSION + " - " + ExtGlobalConfig.DefinedCodename + " - " + ExtGlobalConfig.DefinedUid);
            GlobalLogger.log("Creating Operation window");
            operationWindow = new EternalHushWindow(WindowConstants.MAIN_WINDOW);
            GlobalLogger.log("Creating Settings window");
            settingsWindow = new EternalHushWindow(WindowConstants.SETTINGS_WINDOW);

            tabPanel.addTab("Operation", iconLoader.loadIcon("images/terminal.png", 32, 32), operationWindow);
            for (BasePluginInterface plugin : GlobalVariables.pluginList) {
                String iconPath = plugin.getPluginIcon();
                GlobalLogger.log("Adding plugin tab: " + plugin.getPluginName());
                if (plugin.isShowedByDefault())
                    tabPanel.addTab(plugin.getPluginName(), iconLoader.loadIcon(iconPath, plugin.getClass(), 32, 32), plugin.getPluginTabPanel());

            }
            //tabPanel.addTab("ClingySpider", iconLoader.loadIcon("images/network.png", 32, 32), new ClingySpider());
            tabPanel.addTab("System", iconLoader.loadIcon("images/settings.png", 32, 32), settingsWindow);

            frame.add(tabPanel);
            frame.setSize(GlobalVariables.screenSize.width / 2 + 230, GlobalVariables.screenSize.height / 2 + 150);
            frame.setLocation(GlobalVariables.screenSize.width / 4, GlobalVariables.screenSize.height / 4);

            GlobalVariables.loadScreen.updateStatus("Finishing", 90);
            GlobalVariables.loadScreen.updateStatus("Done", 100);

            GlobalVariables.loadScreen.setEnabled(false);
            GlobalVariables.loadScreen.setVisible(false);

            frame.setVisible(true);
            frame.pack();
            GlobalLogger.log("Calling postInit");
            postInit();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void postInit() throws IOException, InterruptedException {
        StatusEventSource.initCompleted();
    }
}
