package eternalhush.main;

import eternalhush.console.CommandLogger;
import eternalhush.gui.AutorunPanel;
import eternalhush.gui.BasicFrame;
import eternalhush.gui.LoadScreen;
import eternalhush.gui.TabPanel;
import eternalhush.manager.CommonModule;
import eternalhush.manager.MetaInfo;
import eternalhush.plugin.BasePluginInterface;

import javax.swing.*;
import java.awt.*;
import java.util.ArrayList;
import java.util.HashMap;

public abstract class GlobalVariables {
    public static final String VERSION = ExtGlobalConfig.DefinedVersion;
    public static ArrayList<CommonModule> commonModuleList = new ArrayList<>();
    public static BasicFrame rootFrame;
    public static Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    public static TabPanel rootTabPanel;
    public static TabPanel operationPanel;
    public static LoadScreen loadScreen;
    public static JWindow operationInfoWindow;
    public static CommandLogger GlobalOperCommandLogger;
    public static ArrayList<BasePluginInterface> pluginList = new ArrayList<>();
    public static ArrayList<MetaInfo> metaModuleList = new ArrayList<>();
    public static HashMap globalEnv = new HashMap<String, String>();

    public abstract static class CriticalUiComponents {
        public static AutorunPanel autorunPanel;
    }

    public abstract static class OperationConf {
        public static String operPath;
        public static String operCmdLogPath;
        public static Boolean initDone = false;
        public static Boolean postInitDone = false;
        public static Boolean loadHistory = true;
        public static Boolean loadLog = true;
        public static Boolean loadAutorestart = true;
    }
}
