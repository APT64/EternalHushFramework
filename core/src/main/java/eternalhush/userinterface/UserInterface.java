package eternalhush.userinterface;

import eternalhush.console.CommandHandler;
import eternalhush.console.ConsoleManager;
import eternalhush.console.OperationConsole;
import eternalhush.dlcore.CoreConnector;
import eternalhush.dlcore.OperationHandler;
import eternalhush.events.ConsoleEventSource;
import eternalhush.gui.IconLoader;
import eternalhush.main.GlobalVariables;
import eternalhush.main.SettingsLoader;
import eternalhush.manager.BuiltInTaskFlags;

import java.awt.*;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.ArrayList;
import java.util.Properties;
import java.util.Scanner;

public class UserInterface {

    public void print_critical(String str, int consoleId, String uid) {
        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);
        if (consoleInstance == null) {
            consoleInstance = ConsoleManager.getInstance(get_first_available());
            if (consoleInstance == null) return;
        }
        String msgText = str;
        consoleInstance.print(msgText, Color.decode(SettingsLoader.getKeyValue("error_color")));
    }

    public void print_info(String str, int consoleId, String uid) {
        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);
        if (consoleInstance == null) {
            return;
        }
        String msgText = str;
        consoleInstance.printDefault(msgText);
    }

    public void echo(int consoleId, String uid, String str, int type) {
        OperationConsole consoleInstance = null;

        if (consoleId < 0) {
            ConsoleEventSource.statusUpdated(str);
            return;
        } else {
            consoleInstance = ConsoleManager.getInstance(consoleId);

        }
        if (consoleInstance == null) {
            return;
        }
        String msgText = str;
        int msgType = type;
        String endl = "\n";

        if ((msgType & MessagesConstants.ECHO_NOENDL) != 0) {
            msgType -= MessagesConstants.ECHO_NOENDL;
            endl = "";
        }

        if (uid != null && (CoreConnector.Export.GetTaskFlags(uid) & BuiltInTaskFlags.SILENT) != 0) return;

        if (msgType == MessagesConstants.ECHO_DEFAULT) {
            consoleInstance.printDefault(msgText + endl);
        } else if (msgType == MessagesConstants.ECHO_GOOD) {
            consoleInstance.printSuccess(msgText + endl);
        } else if (msgType == MessagesConstants.ECHO_ERROR) {
            consoleInstance.printError(msgText + endl);
        } else if (msgType == MessagesConstants.ECHO_WARNING) {
            consoleInstance.printWarning(msgText + endl);
        }
    }

    public String dialog(int consoleId, String uid, String str, String input_type, String default_text) {
        if (consoleId < 0) return null;
        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);
        if (consoleInstance == null) {
            return null;
        }
        boolean is_task_killer_parent = consoleInstance.task_killer_invoked;
        String msgText = str;
        String defaultText = "";
        String inputOption = default_text;

        if (uid != null && (CoreConnector.Export.GetTaskFlags(uid) & BuiltInTaskFlags.SILENT) != 0) return null;

        if (default_text != null) {
            defaultText = "  (Default= " + default_text + ")";
        }
        consoleInstance.printDefault(msgText + defaultText + "\n");

        Scanner scanner = consoleInstance.getDefaultScanner();
        //Scanner scanner = new Scanner(consoleInstance.redirectInputStream);
        while (true) {
            try {
                //      consoleInstance.redirectNextLine();
            } catch (Exception e) {
            }
            if (scanner.hasNextLine()) {
                inputOption = scanner.nextLine();
                if (input_type.equals("INT") && default_text == null) {
                    try {
                        Integer.parseInt(inputOption);
                        break;
                    } catch (Exception e) {
                        consoleInstance.printError("Non Int input\n");
                    }
                } else {
                    break;
                }
            }
        }
        if (inputOption.equals("")) {
            inputOption = default_text;
        }
        consoleInstance.printDefault(inputOption + "\n");
        consoleInstance.clrQueryText();
        return inputOption;
    }

    public int option(int consoleId, String uid, String str, int default_type) {
        int option;
        if (consoleId < 0) return -1;
        String inputOption = "";
        String default_type_text;
        String defaultText;

        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);
        if (consoleInstance == null) {
            return -1;
        }
        if (uid != null && (CoreConnector.Export.GetTaskFlags(uid) & BuiltInTaskFlags.SILENT) != 0) return -1;
        boolean is_task_killer_parent = consoleInstance.task_killer_invoked;
        String msgText = str;

        if (default_type == MessagesConstants.OPTION_NO) {
            defaultText = "NO";
        } else if (default_type == MessagesConstants.OPTION_YES) {
            defaultText = "YES";
        } else {
            defaultText = "CANCEL";
        }

        default_type_text = "  (Default= " + defaultText + ")";

        Scanner scanner = consoleInstance.getDefaultScanner();

        consoleInstance.printDefault(msgText + default_type_text + "\n");
        consoleInstance.setQueryText(msgText + default_type_text);
        option = default_type;
        while (true) {
            if (scanner.hasNextLine()) {
                inputOption = scanner.nextLine();
            }
            if (inputOption.equalsIgnoreCase("cancel")) {
                option = MessagesConstants.OPTION_CANCEL;
                break;
            } else if (inputOption.equalsIgnoreCase("yes")) {
                option = MessagesConstants.OPTION_YES;
                break;
            } else if (inputOption.equalsIgnoreCase("no")) {
                option = MessagesConstants.OPTION_NO;
                break;
            } else if (inputOption.equals("")) {
                option = default_type;
                inputOption = defaultText;
                break;
            }
            consoleInstance.printError("Unknown option \"" + inputOption + "\". Please, type \"Yes\", \"No\" or \"Cancel\"\n");

        }
        consoleInstance.printDefault(inputOption.toUpperCase() + "\n");
        consoleInstance.clrQueryText();
        return option;
    }

    public void run(int consoleId, String uid, String str, int flag) {
        OperationConsole consoleInstance = ConsoleManager.getInstance(consoleId);
        if (consoleInstance == null) {
            return;
        }
        String Command = str;
        int Option = flag;

        if (Option == MessagesConstants.RUN_SILENT) {
            consoleInstance.outEnable(false);
        }
        CommandHandler handler = new CommandHandler();
        if (consoleInstance.isSessionLocked()) {
            String lockname = consoleInstance.getLockedModuleName();
            handler.CommonHandler(Command, lockname, consoleInstance, 0);
        } else {
            handler.CommonHandler(Command, consoleInstance, 0);
        }
        consoleInstance.outEnable(true);
    }

    public void update_hostname(int consoleId, String uid, String hostname) {
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return;
        }
        operationConsole.setHostname(hostname);
    }

    public void set_env(int consoleId, String uid, String variable, String value) {
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return;
        }
        if (value == null) {
            value = "";
        }
        operationConsole.putEnvVar(variable, value);
    }

    @Deprecated
    public void set_global_env(String variable, String value) {
        if (value == null) {
            value = "";
        }
        try {
            GlobalVariables.globalEnv.put(variable, value);
            Properties props = new Properties();
            props.load(new FileReader(GlobalVariables.OperationConf.operPath + "/env/global.env"));
            props.put(variable, value);
            props.store(new FileWriter(GlobalVariables.OperationConf.operPath + "/env/global.env"), null);
        } catch (Exception e) {
            System.out.println(e.getLocalizedMessage());
        }
    }

    public String get_env(int consoleId, String uid, String variable) {
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return null;

        }
        if (variable != null) {
            String value = (String) operationConsole.getEnv().get(variable);
            if (value == null) {
                return "";
            }
            return value;
        }
        return null;
    }

    @Deprecated
    public String get_global_env(String variable) {
        if (variable != null) {
            String value = (String) GlobalVariables.globalEnv.get(variable);
            if (value == null) {
                return "";
            }
            return value;
        }
        return null;
    }

    public void lock_session(int consoleId, String uid, String module_name) {
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return;
        }
        if (module_name.length() > 0) {
            operationConsole.setSessionLock(true, module_name);
        }
    }

    public void unlock_session(int consoleId, String uid) {
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return;
        }
        operationConsole.setSessionLock(false, "");
    }

    public String get_lock_name(int consoleId, String uid) {
        OperationConsole instance = ConsoleManager.getInstance(consoleId);
        if (instance == null) {
            return null;
        }
        if (!instance.isSessionLocked()) return "";
        return instance.getLockedModuleName();
    }

    public int get_first_available() {
        OperationConsole console = ConsoleManager.getFirstDefault();
        if (console == null) {
            console = new OperationConsole();
            GlobalVariables.operationPanel.addTab(console.getHostname() + " [" + console.getConsoleId() + "]", new IconLoader().loadIcon("images/tv.png", 32, 32), console);
        }
        return console.getConsoleId();
    }

    public void suppress_msg_type(int consoleId, String uid, int type) {
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return;
        }
        operationConsole.outEnableFiltered(type, true);
    }

    public void disable_suppress(int consoleId, String uid, int type) {
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return;
        }
        operationConsole.outEnableFiltered(type, false);
    }

    public boolean add_crypto_key(int consoleId, String key, String key2, String key_name, String key_type, String algorithm){
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return false;
        }
        OperationHandler handler = new OperationHandler();
        ArrayList<OperationHandler.CryptoKeyInfo> keys = handler.QueryKeyStorage();
        for (OperationHandler.CryptoKeyInfo ckey : keys){
            if (key_name.equals(ckey.key_name)) {
                operationConsole.printError(String.format("Key with name '%' already exists in database.", key_name));
                return false;
            }
        }
        OperationHandler.CryptoKeyInfo add_key = new OperationHandler.CryptoKeyInfo();
        add_key.key_type = key_type;
        add_key.key_name = key_name;
        add_key.crypto_blob = key;
        add_key.crypto_blob2 = key2;
        add_key.algorithm = algorithm;
        handler.addNewCryptoKey(add_key);
        return true;
    }

    public ArrayList<String> query_key_storage(int consoleId){
        OperationConsole operationConsole = ConsoleManager.getInstance(consoleId);
        if (operationConsole == null) {
            return null;
        }
        OperationHandler handler = new OperationHandler();
        ArrayList<String> result = new ArrayList<>();
        ArrayList<OperationHandler.CryptoKeyInfo> keys = handler.QueryKeyStorage();
        for (OperationHandler.CryptoKeyInfo ckey : keys) {
            String set = String.format("%s$$KSEP$$%s$$KSEP$$%s$$KSEP$$%s$$KSEP$$%s", ckey.key_name, ckey.crypto_blob, ckey.crypto_blob2, ckey.algorithm, ckey.key_type);
            result.add(set);
        }
        return result;
    }
}
