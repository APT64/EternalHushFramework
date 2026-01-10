package eternalhush.console;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;
import com.sun.jna.Pointer;
import java.awt.AWTException;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Font;
import java.awt.Robot;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.io.PrintStream;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;
import java.util.Scanner;
import java.util.UUID;
import javax.swing.BorderFactory;
import javax.swing.ImageIcon;
import javax.swing.JInternalFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JTextPane;
import javax.swing.SwingUtilities;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameListener;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;
import eternalhush.dlcore.CoreConnector;
import eternalhush.dlcore.OperationHandler;
import eternalhush.events.ConsoleEventListener;
import eternalhush.events.ConsoleEventSource;
import eternalhush.events.StatusEventListener;
import eternalhush.events.StatusEventSource;
import eternalhush.gui.IconLoader;
import eternalhush.gui.TabPanel;
import eternalhush.main.GlobalLogger;
import eternalhush.main.GlobalVariables;
import eternalhush.main.SettingsLoader;

public class OperationConsole extends JInternalFrame implements Runnable {
    public PipedInputStream redirectInputStream;
    public boolean forceabortexecution = false;
    public boolean task_killer_invoked = false;
    PipedOutputStream outputFromField;
    PipedInputStream inputFromField;
    PipedOutputStream redirectOutputStream;
    PrintStream redirectPrintStream;
    private ErrorPrinter errorPrinter;
    private IconLoader iconLoader;
    private JTextPane outputArea;
    private JTextField inputField;
    private StatusPanel statusPanel;
    private JLabel queryLabel;
    private JPanel queryPanel;
    private JLabel cmdStatus;
    private String logPath;
    private String envPath;
    private Scanner fieldInput;
    private PrintStream fieldOutput;
    private JScrollPane scrollPanel;
    private JPanel inputPanel;
    private CommandHandler handler;
    private ArrayList<String> commandHistory;
    private int commandHistoryIndex = 1;
    private int ConsoleId;
    private String ConsoleHostname = this.getLocalHostname();
    private Map ConsoleEnvironment;
    private String HandledCommand;
    private boolean detached = false;
    private boolean isUserInput = false;
    private boolean outputEnabled = true;
    private boolean logging = true;
    private boolean suppressDefaultEcho = false;
    private boolean suppressGoodEcho = false;
    private boolean suppressErrorEcho = false;
    private boolean suppressWarningEcho = false;
    private OperationHandler db_handler;
    private boolean lockFlag = false;
    private String lockedModule;
    private String currentTaskUID;
    private ConsoleLogger logger = new ConsoleLogger(this);
    private int lastCommandId = 0;

    public OperationConsole() {
        super("", false, true, false, true);
        this.setTitle(this.ConsoleHostname + " -- EternalHush Framework " + GlobalVariables.VERSION);
        this.ConsoleId = ConsoleManager.getNewConsoleId();
        this.ConsoleEnvironment = new HashMap();
        this.ConsoleEnvironment.put("CONSOLE_ID", String.valueOf(this.ConsoleId));
        this.ConsoleEnvironment.put("CONSOLE_HOSTNAME", this.ConsoleHostname);
        this.setLayout(new BorderLayout());
        this.iconLoader = new IconLoader();
        this.outputArea = new JTextPane();
        this.inputField = new JTextField();
        this.inputPanel = new JPanel();
        this.commandHistory = new ArrayList();
        this.queryLabel = new JLabel();
        this.queryLabel.setVisible(true);
        this.queryPanel = new JPanel(new BorderLayout());
        this.queryPanel.setVisible(false);
        this.handler = new CommandHandler();
        this.db_handler = new OperationHandler();
        this.statusPanel = new StatusPanel();
        this.errorPrinter = new ErrorPrinter();
        this.inputPanel.setLayout(new BorderLayout());
        this.inputPanel.setFocusable(false);
        this.outputArea.setEditable(false);
        this.outputArea.setFocusable(true);
        this.outputArea.setBackground(Color.BLACK);
        this.inputField.setBackground(Color.BLACK);
        this.outputArea.setForeground(Color.WHITE);
        this.inputField.setForeground(Color.WHITE);
        this.inputField.setCaretColor(Color.WHITE);
        this.inputField.setFocusTraversalKeysEnabled(false);
        this.outputArea.setFont(new Font(SettingsLoader.getKeyValue("console_font"), 0, Integer.parseInt(SettingsLoader.getKeyValue("console_font_size"))));
        this.inputField.setFont(new Font(SettingsLoader.getKeyValue("console_input_font"), 0, Integer.parseInt(SettingsLoader.getKeyValue("console_input_font_size"))));
        this.inputField.setBorder(BorderFactory.createLineBorder(Color.BLACK, 5));
        this.outputArea.setBorder(BorderFactory.createLineBorder(Color.BLACK, 5));
        this.queryPanel.add(this.queryLabel, "West");
        this.queryLabel.setFont(new Font(SettingsLoader.getKeyValue("console_font"), 0, 18));
        this.cmdStatus = new JLabel();
        this.inputPanel.add(this.queryPanel, "First");
        this.inputPanel.add(this.statusPanel, "Last");
        this.inputPanel.add(this.inputField, "Center");
        this.inputPanel.add(this.cmdStatus, "West");
        this.scrollPanel = new JScrollPane(this.outputArea);
        this.scrollPanel.setVerticalScrollBarPolicy(22);
        this.add(this.inputPanel, "Last");
        this.add(this.scrollPanel, "Center");
        this.outputFromField = new PipedOutputStream();
        this.inputFromField = new PipedInputStream();

        try {
            this.outputFromField.connect(this.inputFromField);
        } catch (IOException var5) {
        }

        this.fieldInput = new Scanner(this.inputFromField);
        this.fieldOutput = new PrintStream(this.outputFromField);
        this.redirectInputStream = new PipedInputStream();

        try {
            this.redirectOutputStream = new PipedOutputStream(this.redirectInputStream);
        } catch (IOException var4) {
        }

        this.redirectPrintStream = new PrintStream(this.redirectOutputStream);
        this.HandledCommand = "";
        this.inputField.addKeyListener(new KeyListener() {
            public void keyPressed(KeyEvent keyEvent) {
                String text;
                if (keyEvent.getKeyCode() == 10) {
                    text = OperationConsole.this.inputField.getText();
                    OperationConsole.this.fieldOutput.println(text);
                    OperationConsole.this.commandHistory.add(text);
                    OperationConsole.this.db_handler.AddHistory(OperationConsole.this.ConsoleId, text, (new SimpleDateFormat("HH:mm:ss")).format(Calendar.getInstance().getTime()));
                    OperationConsole.this.inputField.setText("");
                    synchronized(OperationConsole.this.inputFromField) {
                        OperationConsole.this.inputFromField.notify();
                    }
                }

                Thread var10001;
                if (keyEvent.getKeyCode() == 9) {
                    var10001 = new Thread(() -> {
                        try {
                            new ConsoleAutoComplete(OperationConsole.this.inputField.getText(), OperationConsole.this);
                        } catch (IOException var2) {
                            var2.printStackTrace();
                        }

                    }) {
                        {
                            this.start();
                        }
                    };
                }

                if (keyEvent.getKeyCode() == 67 && keyEvent.isControlDown() && OperationConsole.this.getCurrentTaskUID().length() > 0) {
                    text = OperationConsole.this.inputField.getSelectedText();
                    if (text == null) {
                        var10001 = new Thread(() -> {
                            try {
                                OperationConsole.this.task_killer_invoked = true;
                                new TaskKiller(OperationConsole.this);
                                OperationConsole.this.task_killer_invoked = false;
                            } catch (IOException var2) {
                            }

                        }) {
                            {
                                this.start();
                            }
                        };
                    }
                }

                if (keyEvent.getKeyCode() == 38 && OperationConsole.this.commandHistoryIndex - 1 < OperationConsole.this.commandHistory.size()) {
                    OperationConsole.this.inputField.setText((String)OperationConsole.this.commandHistory.get(OperationConsole.this.commandHistory.size() - OperationConsole.this.commandHistoryIndex));
                    ++OperationConsole.this.commandHistoryIndex;
                }

                if (keyEvent.getKeyCode() == 40 && OperationConsole.this.commandHistoryIndex - 1 > 1) {
                    --OperationConsole.this.commandHistoryIndex;
                    OperationConsole.this.inputField.setText((String)OperationConsole.this.commandHistory.get(OperationConsole.this.commandHistory.size() - OperationConsole.this.commandHistoryIndex + 1));
                }

            }

            public void keyReleased(KeyEvent keyEvent) {
            }

            public void keyTyped(KeyEvent keyEvent) {
            }
        });
        this.setCmdStatus_Running();
        ConsoleManager.registerInstance(this);
        Thread consoleThread = new Thread(this);
        consoleThread.start();
        System.setErr(new PrintStream(this.errorPrinter));
        ConsoleEventSource.statusUpdated(String.format("Initializing new instance"));
        ConsoleEventSource.addEventListener(new ConsoleEventListener() {
            public void onConsoleTextChanged(OperationConsole console) {
            }

            public void onStatusUpdated(String text) {
            }

            public void onAllInstancesReady() {
                OperationConsole.this.setActive(true);
            }

            public void onInstanceReady(OperationConsole console) {
            }

            public void onCommandFinished(OperationConsole console) {
            }

            public void onSessionLocked(OperationConsole console) {
            }
        });
        StatusEventSource.addEventListener(new StatusEventListener() {
            public void onInitCompleted() {
                GlobalLogger.log("Calling onInitCompleted routines");
                if (SettingsLoader.getKeyValue("autorun_support").equals("true")) {
                    GlobalLogger.log("Running autorun script");
                    CoreConnector.Export.RunScript("autorun.py", OperationConsole.this.ConsoleId, 0L, 0, (Pointer)null, UUID.randomUUID().toString());
                }

                if (GlobalVariables.OperationConf.loadHistory) {
                    OperationConsole.this.setLogging(false);
                    GlobalLogger.log("Loading previous history");
                    OperationConsole.this.printDefault("Loading previous history...\n");
                    OperationConsole.this.initHistory();
                }

                if (GlobalVariables.OperationConf.loadLog) {
                    OperationConsole.this.setLogging(false);
                    GlobalLogger.log("Loading log file");
                    OperationConsole.this.printDefault("Loading log file...\n");
                    OperationConsole.this.loadLogFile();
                }

                OperationConsole.this.setLogging(false);
                GlobalLogger.log("Loading environment file");
                OperationConsole.this.printDefault("Loading env file...\n");
                OperationConsole.this.loadEnvFile();
                if (GlobalVariables.OperationConf.loadAutorestart) {
                    OperationConsole.this.setLogging(false);
                    GlobalLogger.log("Loading autorestart tasks");
                    OperationConsole.this.printDefault("Loading autorestart tasks...\n");

                    try {
                        Thread.sleep(500L);
                    } catch (InterruptedException var2) {
                        var2.printStackTrace();
                    }

                    OperationConsole.this.runAutostart();
                }

                OperationConsole.this.setLogging(true);
                OperationConsole.this.setCmdStatus_Idle();
                ConsoleManager.instanceReady(OperationConsole.this.getConsoleId());
            }

            public void onPostInitCompleted() {
            }
        });
        this.addInternalFrameListener(new InternalFrameListener() {
            public void internalFrameOpened(InternalFrameEvent internalFrameEvent) {
            }

            public void internalFrameClosing(InternalFrameEvent e) {
                ConsoleManager.startInstanceRemoving(OperationConsole.this.ConsoleId);
            }

            public void internalFrameClosed(InternalFrameEvent internalFrameEvent) {
            }

            public void internalFrameIconified(InternalFrameEvent internalFrameEvent) {
            }

            public void internalFrameDeiconified(InternalFrameEvent internalFrameEvent) {
            }

            public void internalFrameActivated(InternalFrameEvent internalFrameEvent) {
            }

            public void internalFrameDeactivated(InternalFrameEvent internalFrameEvent) {
            }
        });
        if (GlobalVariables.OperationConf.initDone) {
            GlobalLogger.log("Instance is new, initializing");
            if (SettingsLoader.getKeyValue("autorun_support").equals("true")) {
                GlobalLogger.log("Running autorun script");
                CoreConnector.Export.RunScript("autorun.py", this.ConsoleId, 0L, 0, (Pointer)null, UUID.randomUUID().toString());
            }

            String rnd = UUID.randomUUID().toString();
            OperationHandler.InstanceInfo info = new OperationHandler.InstanceInfo();
            info.envfile = GlobalVariables.OperationConf.operPath + "/env/environment-" + rnd + ".env";
            info.name = this.getHostname();
            info.console_id = this.getConsoleId();
            info.logpath = GlobalVariables.OperationConf.operPath + "/Logs/operation_log-" + rnd + ".xml";
            info.last_cid = this.getLastCommandId();
            this.setLogPath(info.logpath);
            this.setEnvPath(info.envfile);
            this.putEnvVar("OPERATION_PATH", GlobalVariables.OperationConf.operPath);
            (new OperationHandler()).SaveInstance(info);
            this.loadEnvFile();
            ConsoleManager.instanceReady(this.getConsoleId());
        }

    }

    public boolean getDetached() {
        return this.detached;
    }

    public void setDetached(boolean state) {
        this.detached = state;
    }

    private void initHistory() {
        OperationHandler handler = new OperationHandler();
        if (!handler.isTableExists("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db", "History")) {
            handler.initConsoleHistory();
        } else {
            this.commandHistory = handler.getConsoleHistory(this.getConsoleId());
        }

    }

    private void runAutostart() {
        GlobalLogger.log("Running autorestart tasks");
        ArrayList<String> autostart_list = this.db_handler.getRestartTask(this.getConsoleId());
        if (autostart_list != null) {
            Iterator var2 = autostart_list.iterator();

            while(var2.hasNext()) {
                String cmd = (String)var2.next();
                this.handler.CommonHandler(cmd, this, 1);

                try {
                    Thread.sleep(400L);
                } catch (Exception var5) {
                    var5.printStackTrace();
                }
            }
        }

    }

    private void loadLogFile() {
        GlobalLogger.log("Loading console log file");
        XmlMapper xmlMapper = new XmlMapper();
        if (!(new File(this.getLogPath())).isFile()) {
            this.setLogging(false);
            GlobalLogger.log("No logfile detected!");
            this.printWarning("No logfile detected!\n");
            this.setLogging(true);
        } else {
            try {
                String moduleContent = Files.readString(Paths.get(this.getLogPath()));
                TextLog module = (TextLog)xmlMapper.readValue(moduleContent, TextLog.class);
                Iterator var4 = module.TextEntry.iterator();

                while(var4.hasNext()) {
                    TextEntry entry = (TextEntry)var4.next();
                    if (entry.status == 0) {
                        this.printDefault(entry.text);
                    } else if (entry.status == 2) {
                        this.printError(entry.text);
                    } else if (entry.status == 1) {
                        this.printSuccess(entry.text);
                    } else if (entry.status == 3) {
                        this.printWarning(entry.text);
                    }
                }
            } catch (JsonProcessingException var6) {
                var6.printStackTrace();
            } catch (IOException var7) {
                var7.printStackTrace();
            }
        }

    }

    private void loadEnvFile() {
        GlobalLogger.log("Loading console environment file");
        Properties env = new Properties();
        if (!(new File(this.getEnvPath())).isFile()) {
            this.setLogging(false);
            this.printWarning("No envfile detected!\n");
            GlobalLogger.log("No envfile detected!");
            env.putAll(this.getEnv());

            try {
                env.store(new FileWriter(this.getEnvPath()), (String)null);
                this.putEnvVar("OPERATION_PATH", GlobalVariables.OperationConf.operPath);
            } catch (Exception var4) {
            }

            this.setLogging(true);
        } else {
            try {
                env.load(new FileInputStream(this.getEnvPath()));
                Iterator var2 = env.stringPropertyNames().iterator();

                while(var2.hasNext()) {
                    String name = (String)var2.next();
                    this.getEnv().put(name, env.getProperty(name));
                }
            } catch (Exception var5) {
                System.out.println(var5.getLocalizedMessage());
            }
        }

    }

    public void clearLogFile() {
        this.logger.clearLogger();
    }

    public JTextPane getOutputArea() {
        return this.outputArea;
    }

    public void setActive(boolean flag) {
        this.inputField.setEnabled(flag);
        this.inputField.setFocusable(flag);
        this.inputField.setEditable(flag);
        this.inputField.repaint();
    }

    public void printWarning(String info) {
        if (!this.suppressWarningEcho) {
            if (this.logging) {
                this.logger.writeIoLog(3, info);
            }

            this.print("- " + info, Color.decode(SettingsLoader.getKeyValue("warn_color")));
        }
    }

    public void printError(String info) {
        if (!this.suppressErrorEcho) {
            if (this.logging) {
                this.logger.writeIoLog(2, info);
            }

            this.print("- " + info, Color.decode(SettingsLoader.getKeyValue("error_color")));
        }
    }

    public void printSuccess(String info) {
        if (!this.suppressGoodEcho) {
            if (this.logging) {
                this.logger.writeIoLog(1, info);
            }

            this.print("- " + info, Color.decode(SettingsLoader.getKeyValue("good_color")));
        }
    }

    public void printDefault(String info) {
        if (!this.suppressDefaultEcho) {
            if (this.logging) {
                this.logger.writeIoLog(0, info);
            }

            this.print(info, Color.decode(SettingsLoader.getKeyValue("default_color")));
        }
    }

    public void emulateUserInput(String input) {
        this.setInputText(input);
        Robot robot = null;

        try {
            robot = new Robot();
        } catch (AWTException var4) {
            var4.printStackTrace();
        }

        this.inputField.requestFocusInWindow();
        robot.keyPress(10);
    }

    public void setLogging(boolean state) {
        this.logging = state;
    }

    public void print(String info, Color color) {
        if (ConsoleManager.isInstanceReady(this.getConsoleId())) {
            ConsoleEventSource.consoleTextChanged(this);
        }

        if (this.outputEnabled) {
            StyledDocument document = this.outputArea.getStyledDocument();
            SimpleAttributeSet attributeSet = new SimpleAttributeSet();
            StyleConstants.setBackground(attributeSet, Color.BLACK);
            StyleConstants.setForeground(attributeSet, color);

            try {
                document.insertString(document.getLength(), info, attributeSet);
                this.outputArea.setCaretPosition(document.getLength());
            } catch (Exception var6) {
                System.out.println(var6);
            }
        }

    }

    public Scanner getDefaultScanner() {
        return this.fieldInput;
    }

    public void outEnable(boolean flag) {
        this.outputEnabled = flag;
    }

    public void outEnableFiltered(int type, boolean flag) {
        if (type == 0) {
            this.suppressDefaultEcho = flag;
        }

        if (type == 1) {
            this.suppressGoodEcho = flag;
        }

        if (type == 2) {
            this.suppressErrorEcho = flag;
        }

        if (type == 3) {
            this.suppressWarningEcho = flag;
        }

    }

    public String getHostname() {
        return this.ConsoleHostname;
    }

    public void setHostname(String hostname) {
        this.putEnvVar("CONSOLE_HOSTNAME", hostname);
        this.ConsoleHostname = hostname;
        if (!this.detached) {
            TabPanel tabPanel = (TabPanel)SwingUtilities.getAncestorOfClass(TabPanel.class, this);
            tabPanel.setTitleAt(tabPanel.indexOfComponent(this), hostname + " [" + this.ConsoleId + "]");
        }

        this.setTitle(this.ConsoleHostname + " -- EternalHush Framework " + GlobalVariables.VERSION);
        OperationHandler.InstanceInfo info = new OperationHandler.InstanceInfo();
        info.logpath = this.logPath;
        info.name = this.ConsoleHostname;
        info.console_id = this.getConsoleId();
        info.envfile = this.envPath;
        info.lockname = this.lockedModule;
        info.last_cid = this.getLastCommandId();
        this.db_handler.SaveInstance(info);
    }

    public int getConsoleTabId() {
        if (!this.detached) {
            TabPanel tabPanel = (TabPanel)SwingUtilities.getAncestorOfClass(TabPanel.class, this);
            return tabPanel.indexOfComponent(this);
        } else {
            return -1;
        }
    }

    public String getLogPath() {
        return this.logPath;
    }

    public void setLogPath(String path) {
        this.logPath = path;
    }

    public String getEnvPath() {
        return this.envPath;
    }

    public void setEnvPath(String path) {
        this.envPath = path;
    }

    public int getConsoleId() {
        return this.ConsoleId;
    }

    public void setConsoleId(int id) {
        GlobalLogger.log(String.format("Console id changed (%d => %d)", this.ConsoleId, id));
        this.ConsoleId = id;
        this.putEnvVar("CONSOLE_ID", String.valueOf(this.ConsoleId));
    }

    public void setQueryText(String text) {
        this.queryLabel.setText(text);
        this.queryPanel.setVisible(true);
    }

    public void clrQueryText() {
        this.queryPanel.setVisible(false);
    }

    public String getLocalHostname() {
        Map<String, String> env = System.getenv();
        return env.containsKey("COMPUTERNAME") ? (String)env.get("COMPUTERNAME") : (String)env.getOrDefault("HOSTNAME", "UNKNOWN");
    }

    public Map getEnv() {
        return this.ConsoleEnvironment;
    }

    public void setSessionLock(boolean flag, String mod_name) {
        if (flag) {
            GlobalLogger.log(String.format("Locking console session"));
            if (this.lockFlag && this.lockedModule != null && !this.lockedModule.equals(mod_name)) {
                this.printError("Failed to lock session! Has the session already been blocked?");
                GlobalLogger.log(String.format("Failed to lock session! Has the session already been blocked?"));
                return;
            }

            this.lockedModule = mod_name;
            OperationHandler.InstanceInfo info = new OperationHandler.InstanceInfo();
            info.logpath = this.logPath;
            info.name = this.ConsoleHostname;
            info.console_id = this.getConsoleId();
            info.envfile = this.envPath;
            info.lockname = this.lockedModule;
            info.last_cid = this.getLastCommandId();
            this.db_handler.SaveInstance(info);
            ConsoleEventSource.sessionLocked(this);
        } else {
            this.lockedModule = null;
        }

        this.lockFlag = flag;
    }

    public String getCurrentTaskUID() {
        return this.currentTaskUID;
    }

    public void setCurrentTaskUID(String uid) {
        this.currentTaskUID = uid;
    }

    public boolean isSessionLocked() {
        return this.lockFlag;
    }

    public void putEnvVar(String key, String value) {
        if (!GlobalVariables.OperationConf.initDone) {
            GlobalLogger.log("putEnvVar before init, ignoring...");
        } else {
            Map<String, String> envp = this.getEnv();
            envp.put(key, value);

            try {
                FileWriter w = new FileWriter(this.getEnvPath());
                Properties props = new Properties();
                props.putAll(envp);
                props.store(w, (String)null);
                w.close();
            } catch (Exception var6) {
                System.out.println(var6.getLocalizedMessage());
            }

        }
    }

    public void setCmdStatus_Running() {
        ImageIcon i = this.iconLoader.loadIcon("images/loadwheel.gif");
        this.cmdStatus.setIcon(i);
        this.cmdStatus.repaint();
    }

    public void setCmdStatus_Idle() {
        ImageIcon i = this.iconLoader.loadIcon("images/greenarrow.png");
        this.cmdStatus.setIcon(i);
        this.cmdStatus.repaint();
    }

    public int getLastCommandId() {
        return this.lastCommandId;
    }

    public void setLastCommandId(int id) {
        this.lastCommandId = id;
        OperationHandler.InstanceInfo info = new OperationHandler.InstanceInfo();
        info.logpath = this.logPath;
        info.name = this.ConsoleHostname;
        info.console_id = this.getConsoleId();
        info.envfile = this.envPath;
        info.lockname = this.lockedModule;
        info.last_cid = this.getLastCommandId();
        this.db_handler.SaveInstance(info);
    }

    public void redirectNextLine() throws IOException {
        this.isUserInput = true;
    }

    public String getInputText() {
        return this.inputField.getText();
    }

    public void setInputText(String text) {
        this.inputField.setText(text);
        this.inputField.setCaretPosition(text.length());
    }

    public String getLockedModuleName() {
        return this.lockedModule;
    }

    public void run() {
        while(this.fieldInput.hasNextLine()) {
            String linePrefix;
            if (!this.isUserInput) {
                this.HandledCommand = this.fieldInput.nextLine();
                SimpleDateFormat var10000 = new SimpleDateFormat("HH:mm:ss");
                linePrefix = "[" + var10000.format(Calendar.getInstance().getTime()) + "] >> ";
                if (this.outputEnabled) {
                    this.printDefault(linePrefix + this.HandledCommand + "\n");
                }

                if (this.HandledCommand.length() > 0) {
                    if (this.getConsoleId() != -1) {
                        this.setCmdStatus_Running();
                    }

                    if (this.lockFlag) {
                        this.handler.CommonHandler(this.HandledCommand, this.lockedModule, this, 1);
                    } else {
                        this.handler.CommonHandler(this.HandledCommand, this, 1);
                    }

                    if (this.getConsoleId() != -1) {
                        this.setCmdStatus_Idle();
                    }
                }
            } else {
                this.isUserInput = false;
                linePrefix = this.fieldInput.nextLine();
                this.redirectPrintStream.println(linePrefix);
                synchronized(this.redirectPrintStream) {
                    this.redirectPrintStream.notify();
                }
            }
        }

    }

    public ConsoleLogger getDefaultTextLogger() {
        return this.logger;
    }
}