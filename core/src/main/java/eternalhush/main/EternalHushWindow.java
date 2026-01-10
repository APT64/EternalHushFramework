package eternalhush.main;

import eternalhush.console.ConsoleManager;
import eternalhush.console.ConsoleTabPanel;
import eternalhush.console.OperationConsole;
import eternalhush.dlcore.OperationHandler;
import eternalhush.gui.WindowConstants;
import eternalhush.gui.*;
import eternalhush.userinterface.UserInterface;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Properties;

public class EternalHushWindow extends JPanel {

    BasePopupListener listener = e -> {
    };
    private IconLoader iconLoader = new IconLoader();

    public EternalHushWindow(final int type) {
        this.addMouseListener(new MouseAdapter() {

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    showPopupListener(e);
                }
            }

        });
        setLayout(new BorderLayout());
        setType(type);
    }

    public EternalHushWindow(final int type, LayoutManager layout) {
        setLayout(layout);
    }

    void showPopupListener(MouseEvent e) {
        listener.showPopup(e);
    }

    public void setPopupListener(BasePopupListener l) {
        listener = l;
    }

    private Boolean initInstance() {
        OperationHandler handler = new OperationHandler();
        if (!handler.isTableExists("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db", "Instances")) {
            handler.initConsoleInstances();
            return true;
        } else {

            return false;
        }
    }

    private Boolean initGlobalEnv() {
        GlobalLogger.log("Initializing global.env");
        if (!new File(GlobalVariables.OperationConf.operPath + "/env/global.env").isFile()) {
            Properties props = new Properties();
            try {
                props.store(new FileWriter(GlobalVariables.OperationConf.operPath + "/env/global.env"), null);
                new UserInterface().set_global_env("OPERATION_PATH", GlobalVariables.OperationConf.operPath);
                new UserInterface().set_global_env("OPERATOR_HOSTNAME", System.getenv("COMPUTERNAME"));
            } catch (Exception e) {
                System.out.println(e.getLocalizedMessage());
            }
            return true;
        } else {
            return false;
        }
    }

    private Boolean loadGlobalEnv() {
        GlobalLogger.log("Loading global.env");
        if (new File(GlobalVariables.OperationConf.operPath + "/env/global.env").isFile()) {
            Properties props = new Properties();
            try {
                props.load(new FileReader(GlobalVariables.OperationConf.operPath + "/env/global.env"));
                for (final String name : props.stringPropertyNames())
                    GlobalVariables.globalEnv.put(name, props.getProperty(name));
            } catch (Exception e) {
                System.out.println(e.getLocalizedMessage());
                return false;
            }
            return true;
        } else {
            return false;
        }
    }

    private Boolean initQueue() {
        GlobalLogger.log("Initializing autorestart queue");
        OperationHandler handler = new OperationHandler();
        if (!handler.isTableExists("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db", "Tasks")) {
            handler.initRestartQueue();
            return true;
        } else {

            return false;
        }
    }

    private Boolean initKeyStorage() {
        GlobalLogger.log("Initializing cryptokey storage");
        OperationHandler handler = new OperationHandler();
        if (!handler.isTableExists("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db", "KeyStorage")) {
            handler.initKeyStorage();
            return true;
        } else {

            return false;
        }
    }

    private void setType(final int type) {

        switch (type) {
            case WindowConstants.MAIN_WINDOW:
                GlobalLogger.log("Loading operation");
                GlobalVariables.loadScreen.updateStatus("Loading operation", 15);
                OperationHandler handler = new OperationHandler();
                ConsoleTabPanel operationPanel = new ConsoleTabPanel();
                operationPanel.setPopupListener(new BasePopupListener() {
                    @Override
                    public void showPopup(MouseEvent e) {
                        YPopupMenu popupMenu = new YPopupMenu();
                        PopMenuItem rename = new PopMenuItem("Rename", iconLoader.loadIcon("images/text_edit.png", 17, 17));
                        PopMenuItem exit = new PopMenuItem("Exit", iconLoader.loadIcon("images/close.png", 17, 17));
                        exit.addActionListener(new ActionListener() {
                            @Override
                            public void actionPerformed(ActionEvent actionEvent) {
                                ConsoleManager.startSelectedInstanceRemoving();
                            }
                        });
                        rename.addActionListener(new ActionListener() {
                            @Override
                            public void actionPerformed(ActionEvent actionEvent) {
                                String result = JOptionPane.showInputDialog(
                                        EternalHushWindow.this,
                                        "Enter new name:", "Rename", JOptionPane.QUESTION_MESSAGE);
                                OperationConsole currentConsole = (OperationConsole) (operationPanel.getSelectedComponent());
                                GlobalLogger.log(String.format("Changed console name %d (%s => %s)", currentConsole.getConsoleId(), currentConsole.getHostname(), result));
                                currentConsole.setHostname(result);
                            }
                        });
                        popupMenu.add(rename);

                        popupMenu.add(exit);
                        popupMenu.show((JComponent) e.getSource(), e.getX(), e.getY());
                    }
                });
                operationPanel.setTabPlacement(SwingConstants.LEFT);
                GlobalVariables.operationPanel = operationPanel;
                GlobalLogger.log(String.format("Initializing log files & databases"));
                GlobalVariables.loadScreen.updateStatus("Initializing log files & databases", 25);
                new File(GlobalVariables.OperationConf.operPath + "/Logs").mkdirs();
                GlobalLogger.log(String.format("Creating directory %s", GlobalVariables.OperationConf.operPath + "/Logs"));
                new File(GlobalVariables.OperationConf.operPath + "/env").mkdirs();
                GlobalLogger.log(String.format("Creating directory %s", GlobalVariables.OperationConf.operPath + "/env"));
                new File(GlobalVariables.OperationConf.operPath + "/storage").mkdirs();
                GlobalLogger.log(String.format("Creating directory %s", GlobalVariables.OperationConf.operPath + "/storage"));
                initGlobalEnv();
                initQueue();
                initKeyStorage();
                if (!initInstance()) {
                    GlobalLogger.log(String.format("Loading saved instance info"));
                    ArrayList<OperationHandler.InstanceInfo> list = handler.getInstancesInfo();

                    GlobalVariables.loadScreen.updateStatus("Loading global environment", 30);
                    loadGlobalEnv();

                    GlobalVariables.loadScreen.updateStatus("Loading saved instances", 35);

                    for (OperationHandler.InstanceInfo info : list) {
                        OperationConsole consoleInstance = new OperationConsole();
                        consoleInstance.setActive(false);
                        GlobalLogger.log(String.format("Loading saved instance %d %s %s", info.console_id, info.name, info.logpath));
                        consoleInstance.setConsoleId(info.console_id);
                        //load logfile
                        consoleInstance.setLogPath(info.logpath);
                        consoleInstance.getDefaultTextLogger().readPreviousLog();

                        consoleInstance.setEnvPath(info.envfile);
                        consoleInstance.setLastCommandId(info.last_cid);
                        operationPanel.addTab(consoleInstance.getHostname() + " [" + consoleInstance.getConsoleId() + "]", iconLoader.loadIcon("images/tv.png", 32, 32), consoleInstance);
                        consoleInstance.setHostname(info.name);
                        if (info.lockname != null) {
                            GlobalLogger.log(String.format("Locking console session %s", info.lockname));
                            consoleInstance.setSessionLock(true, info.lockname);
                        }
                    }
                } else {
                    GlobalLogger.log(String.format("Initializing new instances"));
                    GlobalVariables.loadScreen.updateStatus("Initializing new instances", 30);
                    for (int i = 0; i < 3; i++) {
                        OperationConsole consoleInstance = ConsoleManager.initializeFullInstance();
                        operationPanel.addTab(consoleInstance.getHostname() + " [" + consoleInstance.getConsoleId() + "]", iconLoader.loadIcon("images/tv.png", 32, 32), consoleInstance);
                    }
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
