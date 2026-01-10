package eternalhush.console;

import eternalhush.dlcore.OperationHandler;
import eternalhush.events.ConsoleEventSource;
import eternalhush.main.GlobalLogger;
import eternalhush.main.GlobalVariables;

import javax.swing.*;
import java.awt.event.WindowEvent;
import java.io.File;
import java.util.ArrayList;
import java.util.UUID;

public abstract class ConsoleManager {
    private static ArrayList<OperationConsole> consoleInstances = new ArrayList<>();
    private static ArrayList<Integer> readyList = new ArrayList<>();
    private static boolean is_all_ready = false;

    public static void registerInstance(OperationConsole instance) {
        GlobalLogger.log(String.format("Registering new console instance %d", instance.getConsoleId()));
        consoleInstances.add(instance);
    }

    public static OperationConsole getInstance(int id) {
        for (OperationConsole console : consoleInstances) {
            if (console.getConsoleId() == id) {
                return console;
            }
        }
        return null;
    }

    public static boolean isInstanceReady(int console_id) {
        for (int id :
                readyList) {
            if (id == console_id) return true;
        }
        return false;
    }

    public static OperationConsole getFirstDefault() {
        for (OperationConsole consoleInstance : consoleInstances) {
            if (!consoleInstance.isSessionLocked()) {
                return consoleInstance;
            }
        }
        return null;
    }

    public static OperationConsole initializeFullInstance() {
        String rnd = UUID.randomUUID().toString();
        OperationConsole consoleInstance = new OperationConsole();
        consoleInstance.setActive(false);
        GlobalLogger.log(String.format("Initializing new console instance %d", consoleInstance.getConsoleId()));
        OperationHandler.InstanceInfo info = new OperationHandler.InstanceInfo();
        info.envfile = GlobalVariables.OperationConf.operPath + "/env/environment-" + rnd + ".env";
        info.name = consoleInstance.getHostname();
        info.console_id = consoleInstance.getConsoleId();
        info.logpath = GlobalVariables.OperationConf.operPath + "/Logs/operation_log-" + rnd + ".xml";
        consoleInstance.setLogPath(info.logpath);
        consoleInstance.setEnvPath(info.envfile);
        consoleInstance.putEnvVar("OPERATION_PATH", GlobalVariables.OperationConf.operPath);
        consoleInstance.setCmdStatus_Idle();
        new OperationHandler().SaveInstance(info);
        return consoleInstance;
    }

    public static int lastInstanceId() {
        int cid = 0;
        if (consoleInstances.size() != 0) {
            cid = consoleInstances.get(consoleInstances.size() - 1).getConsoleId();
        }
        return cid;
    }

    public static int getNewConsoleId() {
        if (consoleInstances.size() == 0) {
            return 0;
        }
        return lastInstanceId() + 1;
    }

    public static int getCount() {
        return consoleInstances.size();
    }

    public static void startSelectedInstanceRemoving() {
        if (GlobalVariables.operationPanel.getTabCount() == 2) {
            JOptionPane.showMessageDialog(GlobalVariables.rootFrame, "Unable to end last session", "Session error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        int result = JOptionPane.showConfirmDialog(GlobalVariables.rootFrame, "Do you want to end this session?", "Confirmation", JOptionPane.YES_NO_OPTION);
        if (result == JOptionPane.YES_OPTION) {

            OperationConsole currentConsole = (OperationConsole) (GlobalVariables.operationPanel.getSelectedComponent());
            GlobalLogger.log(String.format("Removing console instance %d", currentConsole.getConsoleId(), result));
            currentConsole.setLogging(false);
            GlobalLogger.log("Removing associated log file");
            currentConsole.printWarning("Removing associated log file");
            new File(currentConsole.getLogPath()).delete();
            GlobalLogger.log("Removing associated env file");
            currentConsole.printWarning("Removing associated env file\n");
            new File(currentConsole.getEnvPath()).delete();
            GlobalLogger.log("Removing all references in databases");
            currentConsole.printWarning("Removing all references in databases\n");
            new OperationHandler().RemoveAllCidReferencedObjects(currentConsole.getConsoleId());
            //BUGFIX: cyclic adding new eternalhush.console
            if (currentConsole.getConsoleTabId() + 1 == GlobalVariables.operationPanel.getTabCount() - 1) {
                GlobalVariables.operationPanel.setSelectedIndex(currentConsole.getConsoleTabId() - 1);
            }
            GlobalVariables.operationPanel.remove(currentConsole.getConsoleTabId());
        }
    }

    public static void instanceReady(int consoleId) {
        GlobalLogger.log(String.format("New console instance ready %d", consoleId));
        readyList.add(consoleId);
        ConsoleEventSource.instanceReady(getInstance(consoleId));
        if (consoleInstances.size() == readyList.size()) {
            GlobalLogger.log(String.format("All instances ready!"));
            ConsoleEventSource.allInstancesReady();
            is_all_ready = true;
            ConsoleEventSource.statusUpdated("All instances ready");
        }
    }

    public static ArrayList<OperationConsole> getInstanceList() {
        return consoleInstances;
    }

    public static void startInstanceRemoving(int consoleId) {
        OperationConsole currentConsole = getInstance(consoleId);
        if (GlobalVariables.operationPanel.getTabCount() == 2 && !currentConsole.getDetached()) {
            JOptionPane.showMessageDialog(currentConsole, "Unable to end last session", "Session error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        int result = JOptionPane.showConfirmDialog(GlobalVariables.rootFrame, "Do you want to end this session?", "Confirmation", JOptionPane.YES_NO_OPTION);
        if (result == JOptionPane.YES_OPTION) {
            GlobalLogger.log(String.format("Removing console instance %d", currentConsole.getConsoleId(), result));
            currentConsole.setLogging(false);
            GlobalLogger.log("Removing associated log file");
            currentConsole.printWarning("Removing associated log file");
            new File(currentConsole.getLogPath()).delete();
            GlobalLogger.log("Removing associated env file");
            currentConsole.printWarning("Removing associated env file\n");
            new File(currentConsole.getEnvPath()).delete();
            GlobalLogger.log("Removing all references in databases");
            currentConsole.printWarning("Removing all references in databases\n");
            new OperationHandler().RemoveAllCidReferencedObjects(currentConsole.getConsoleId());
            //BUGFIX: cyclic adding new eternalhush.console
            if (currentConsole.getDetached()) {
                JFrame frame = (JFrame) SwingUtilities.windowForComponent(currentConsole);
                frame.dispatchEvent(new WindowEvent(frame, WindowEvent.WINDOW_CLOSING));
                try {
                    Thread.sleep(500);
                } catch (Exception e) {
                }
            }
            if (currentConsole.getConsoleTabId() + 1 == GlobalVariables.operationPanel.getTabCount() - 1) {
                GlobalVariables.operationPanel.setSelectedIndex(currentConsole.getConsoleTabId() - 1);
            }
            GlobalVariables.operationPanel.remove(currentConsole.getConsoleTabId());
        }
    }


    public boolean isAllReady() {
        return is_all_ready;
    }
}
