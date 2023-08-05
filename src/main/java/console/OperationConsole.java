package console;

import core.CoreConnector;
import eternalhush.SettingsLoader;
import events.EternalEventSource;
import events.EternalEventListener;
import gui.IconLoader;

import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.ArrayList;
import java.util.Scanner;

public class OperationConsole extends JPanel implements Runnable {
    private ErrorPrinter errorPrinter;

    private IconLoader iconLoader;

    private JTextPane outputArea;
    private JTextField inputField;
    private StatusPanel statusPanel;

    PipedOutputStream outputFromField;
    PipedInputStream inputFromField;

    private Scanner fieldInput;
    private PrintStream fieldOutput;

    private JScrollPane scrollPanel;
    private JPanel inputPanel;

    private CommandHandler handler;
    private ArrayList<String> commandHistory;
    private int commandHistoryIndex = 1;

    private String ConsoleTarget;

    private String HandledCommand;
    private boolean isUserInput = false;
    private boolean outputEnabled = true;

    public OperationConsole(){
        ConsoleTarget = new String("localhost");

        setLayout(new BorderLayout());
        iconLoader = new IconLoader();
        outputArea = new JTextPane();
        inputField = new JTextField();
        inputPanel = new JPanel();
        commandHistory = new ArrayList<String>();
        handler = new CommandHandler();
        statusPanel = new StatusPanel();

        errorPrinter = new ErrorPrinter(this);

        inputPanel.setLayout(new BorderLayout());
        inputPanel.setFocusable(false);

        outputArea.setEditable(false);
        outputArea.setFocusable(true);

        outputArea.setBackground(Color.BLACK);
        inputField.setBackground(Color.BLACK);
        outputArea.setForeground(Color.WHITE);
        inputField.setForeground(Color.WHITE);

        inputField.setCaretColor(Color.WHITE);

        outputArea.setFont(new Font(SettingsLoader.getKeyValue("console_font"), Font.PLAIN, Integer.valueOf(SettingsLoader.getKeyValue("console_font_size"))));
        inputField.setFont(new Font(SettingsLoader.getKeyValue("console_input_font"), Font.PLAIN, Integer.valueOf(SettingsLoader.getKeyValue("console_input_font_size"))));

        inputField.setBorder(BorderFactory.createLineBorder(Color.BLACK, 5));
        outputArea.setBorder(BorderFactory.createLineBorder(Color.BLACK, 5));

        statusPanel.setGlobalEnabled(false);

        inputPanel.add(statusPanel, BorderLayout.PAGE_END);
        inputPanel.add(inputField, BorderLayout.CENTER);
        inputPanel.add(new JLabel(iconLoader.loadIcon("images/greenarrow_icon.png")), BorderLayout.WEST);

        scrollPanel = new JScrollPane(outputArea);
        scrollPanel.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);

        add(inputPanel, BorderLayout.PAGE_END);

        add(scrollPanel, BorderLayout.CENTER);

        outputFromField = new PipedOutputStream();
        inputFromField = new PipedInputStream();
        try {
            outputFromField.connect(inputFromField);
        } catch (IOException e){}

        fieldInput = new Scanner(inputFromField);
        fieldOutput = new PrintStream(outputFromField);

        HandledCommand = new String();

        inputField.addKeyListener(new KeyListener() {
            @Override
            public void keyPressed(KeyEvent keyEvent) {
                if (keyEvent.getKeyCode() == KeyEvent.VK_ENTER){
                    String text = inputField.getText();
                    fieldOutput.println(text);
                    commandHistory.add(text);
                    inputField.setText("");

                    synchronized(inputFromField) {
                        inputFromField.notify();
                    }
                }

                if (keyEvent.getKeyCode() == KeyEvent.VK_UP && commandHistoryIndex-1 < commandHistory.size()){
                    inputField.setText(commandHistory.get(commandHistory.size()-commandHistoryIndex));
                    commandHistoryIndex++;
                }
                if (keyEvent.getKeyCode() == KeyEvent.VK_DOWN && commandHistoryIndex-1 > 1){
                    commandHistoryIndex--;
                    inputField.setText(commandHistory.get(commandHistory.size()-commandHistoryIndex+1));
                }
            }
            @Override
            public void keyReleased(KeyEvent keyEvent) {}
            @Override
            public void keyTyped(KeyEvent keyEvent) {}
        });

        ConsoleManager.registerInstance(this);

        Thread consoleThread = new Thread(this);
        consoleThread.start();

        System.setErr(new PrintStream(errorPrinter));

        EternalEventSource.addEventListener(new EternalEventListener() {
            @Override
            public void onInitCompleted() {
                if (SettingsLoader.getKeyValue("use_autorun").equals("true")){
                    CoreConnector.Export.RunScript("autorun.py", 0, null);
                }
            }
        });
    }

    public JTextPane getOutputArea(){
        return outputArea;
    }

    public void printWarning(String info){
        print("- " + info, Color.YELLOW);
    }

    public void printError(String info){
        print("- " + info, Color.RED);
    }

    public void printSuccess(String info){
        print("- " + info, Color.GREEN);
    }

    public void printDefault(String info){
        print(info, Color.WHITE);
    }

    public void print(String info, Color color)
    {
        if (outputEnabled) {
            StyledDocument document = outputArea.getStyledDocument();
            SimpleAttributeSet attributeSet = new SimpleAttributeSet();

            StyleConstants.setBackground(attributeSet, Color.BLACK);
            StyleConstants.setForeground(attributeSet, color);

            try {
                document.insertString(document.getLength(), info, attributeSet);
                outputArea.setCaretPosition(document.getLength());
            } catch (Exception e) {
                System.out.println(e);
            }
        }
    }

    public String getTarget(){
        return ConsoleTarget;
    }
    public void setTarget(String target){
        ConsoleTarget = target;
    }

    public Scanner getDefaultScanner(){
        return fieldInput;
    }

    public CommandHandler getDefaultHandler(){
        return handler;
    }

    public void outEnable(boolean flag){
        outputEnabled = flag;
    }

    public void run() {
        while (fieldInput.hasNextLine()) {

            if (!isUserInput) {
                HandledCommand = fieldInput.nextLine();

                String linePrefix = new String("[" + new java.text.SimpleDateFormat("HH:mm:ss").format(java.util.Calendar.getInstance().getTime()) + "] >> ");
                if (outputEnabled) {
                    printDefault(linePrefix + HandledCommand + "\n");
                }
                if (HandledCommand.length() > 0) {
                    handler.CommonHandler(HandledCommand, this);
                }
            }
        }
    }
}

