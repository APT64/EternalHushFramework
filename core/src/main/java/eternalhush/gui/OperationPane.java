package eternalhush.gui;

import eternalhush.main.GlobalLogger;
import eternalhush.main.GlobalVariables;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;

public class OperationPane extends JFrame {
    private JPanel OptionsPanel;
    private JTextField operationPath;
    private JTextField operationCmdLogPath;
    private JButton operationPathBtn;
    private JButton operationCmdLogBtn;
    private JLabel operationPathLabel;
    private JLabel operationCmdLogLabel;
    private JFileChooser fileChooser;
    private JButton goButton;
    private JCheckBox loadHistory;
    private JCheckBox loadLog;
    private JCheckBox loadAutorestart;
    private JLabel histLabel;
    private JLabel logLabel;
    private JLabel autorunLabel;
    private JPanel loadOpts;
    private JPanel operOptionsPanel;

    private JTextField modulesPath;
    private JLabel modulesPathLabel;
    private JButton modulesPathBtn;

    public OperationPane(String[] args) {
        operOptionsPanel = new JPanel();
        OptionsPanel = new JPanel();
        loadOpts = new JPanel(new BorderLayout());
        goButton = new JButton("Go");
        operationPath = new JTextField(new JFileChooser().getFileSystemView().getDefaultDirectory().toString());
        operationCmdLogPath = new JTextField(operationPath.getText() + "\\operation_command_log.xml");
        modulesPath = new JTextField(System.getProperty("user.dir") + "\\modules");
        operationPathBtn = new JButton("Browse...");
        operationCmdLogBtn = new JButton("Browse...");
        modulesPathBtn = new JButton("Browse...");
        operationPathLabel = new JLabel("Operation Path:");
        operationCmdLogLabel = new JLabel("Operation Command Log:");
        modulesPathLabel = new JLabel("Modules Path:");
        fileChooser = new JFileChooser();

        loadAutorestart = new JCheckBox("", true);
        loadHistory = new JCheckBox("", true);
        loadLog = new JCheckBox("", true);
        loadAutorestart.setHorizontalTextPosition(JCheckBox.LEFT);
        loadHistory.setHorizontalTextPosition(JCheckBox.LEFT);
        loadLog.setHorizontalTextPosition(JCheckBox.LEFT);

        histLabel = new JLabel("Load history:");
        autorunLabel = new JLabel("Load autorun tasks:");
        logLabel = new JLabel("Load log files:");

        operOptionsPanel.setBorder(BorderFactory.createTitledBorder("Options"));
        GroupLayout optionsLayout = new GroupLayout(operOptionsPanel);
        operOptionsPanel.setLayout(optionsLayout);

        GroupLayout layout = new GroupLayout(OptionsPanel);
        OptionsPanel.setLayout(layout);

        if (new File("operation.last").isFile()) {
            BufferedReader reader = null;
            try {
                GlobalLogger.log("Reading operation.last");
                reader = new BufferedReader(new FileReader("operation.last"));

                operationPath.setText(reader.readLine());
                operationCmdLogPath.setText(operationPath.getText() + "\\operation_command_log.xml");
                reader.close();
            } catch (Exception e) {

            }
        }

        this.setTitle("EternalHush Operation Center");
        this.setSize(900, 300);
        this.add(OptionsPanel);
        this.setResizable(false);
        this.setLocationRelativeTo(null);
        this.setDefaultCloseOperation(EXIT_ON_CLOSE);

        operationPathBtn.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent actionEvent) {
                fileChooser.setDialogTitle("Choose Operation Folder");
                fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
                int result = fileChooser.showOpenDialog(OperationPane.this);
                if (result == JFileChooser.APPROVE_OPTION) {
                    operationPath.setText(fileChooser.getSelectedFile().toString());
                    operationCmdLogPath.setText(operationPath.getText() + "\\operation_command_log.xml");
                }
            }
        });
        modulesPathBtn.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent actionEvent) {
                fileChooser.setDialogTitle("Choose Modules Folder");
                fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
                int result = fileChooser.showOpenDialog(OperationPane.this);
                if (result == JFileChooser.APPROVE_OPTION) {
                    modulesPath.setText(fileChooser.getSelectedFile().toString());
                }
            }
        });
        operationCmdLogBtn.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent actionEvent) {
                fileChooser.setDialogTitle("Choose Operation Command Log");
                fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
                int result = fileChooser.showOpenDialog(OperationPane.this);
                if (result == JFileChooser.APPROVE_OPTION) {
                    operationCmdLogPath.setText(fileChooser.getSelectedFile().toString() + "\\operation_command_log.xml");
                }
            }
        });
        goButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent var1) {
                try {
                    if (new JFileChooser().getFileSystemView().getDefaultDirectory().toString().equals(operationPath.getText())) {
                        JOptionPane.showMessageDialog(OperationPane.this, "Choose a non-standard directory!");
                        return;
                    }

                    setEnabled(false);
                    setVisible(false);

                    GlobalVariables.OperationConf.operPath = operationPath.getText();
                    GlobalLogger.log("Using operation path " + operationPath.getText());
                    GlobalVariables.OperationConf.operCmdLogPath = operationCmdLogPath.getText();
                    GlobalLogger.log("Using operation log path " + operationCmdLogPath.getText());
                    GlobalVariables.globalEnv.put("MODULES_DIRECTORY", modulesPath.getText());
                    GlobalLogger.log("Using modules directory " + modulesPath.getText());
                    //  if(operationPath.getText().endsWith("\\") || operationPath.getText().endsWith("/") ){
                    //       GlobalVariables.OperationConf.operPath = operationPath.getText().substring(0, operationPath.getText().length()-1);
                    //   }
                    PrintWriter writer = new PrintWriter("operation.last", StandardCharsets.UTF_8);
                    writer.println(operationPath.getText());
                    writer.close();
                    GlobalLogger.log("Last operation saved");
                    GlobalVariables.OperationConf.loadAutorestart = loadAutorestart.isSelected();
                    GlobalVariables.OperationConf.loadHistory = loadHistory.isSelected();
                    GlobalVariables.OperationConf.loadLog = loadLog.isSelected();
                    GlobalLogger.log("Load autorestart tasks: " + GlobalVariables.OperationConf.loadAutorestart);
                    GlobalLogger.log("Load console history: " + GlobalVariables.OperationConf.loadHistory);
                    GlobalLogger.log("Load console logs: " + GlobalVariables.OperationConf.loadLog);
                    GlobalVariables.loadScreen = new LoadScreen(args);

                } catch (Exception e) {
                }
            }
        });
        GlobalLogger.log("Operation Center Panel ready");

        optionsLayout.setHorizontalGroup(optionsLayout.createSequentialGroup()
                .addContainerGap(100, 30000)
                .addGroup(optionsLayout.createParallelGroup()
                        .addComponent(autorunLabel)
                        .addComponent(histLabel)
                        .addComponent(logLabel)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(optionsLayout.createParallelGroup()
                        .addComponent(loadAutorestart)
                        .addComponent(loadHistory)
                        .addComponent(loadLog)
                )
                .addContainerGap(100, 30000)
        );
        optionsLayout.setVerticalGroup(optionsLayout.createSequentialGroup()
                .addGap(10)
                .addGroup(optionsLayout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(autorunLabel)
                        .addComponent(loadAutorestart)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 5, 5)
                .addGroup(optionsLayout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(histLabel)
                        .addComponent(loadHistory)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 5, 5)
                .addGroup(optionsLayout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(logLabel)
                        .addComponent(loadLog)
                )
                .addGap(10)
        );
        layout.setHorizontalGroup(layout.createSequentialGroup()
                .addContainerGap(100, 30000)
                .addGroup(layout.createParallelGroup()
                        .addComponent(modulesPathLabel)
                        .addComponent(operationPathLabel)
                        .addComponent(operationCmdLogLabel)
                        .addComponent(operOptionsPanel)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup()
                        .addComponent(modulesPath, 10, 5000, 30000)
                        .addComponent(operationPath, 10, 5000, 30000)
                        .addComponent(operationCmdLogPath, 10, 5000, 30000)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup()
                        .addComponent(modulesPathBtn)
                        .addComponent(operationPathBtn)
                        .addComponent(operationCmdLogBtn)
                        .addComponent(goButton, 10, 100, 100)
                )

                .addContainerGap(100, 30000)

        );

        layout.setVerticalGroup(layout.createSequentialGroup()
                .addGap(10)
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(modulesPathLabel)
                        .addComponent(modulesPath)
                        .addComponent(modulesPathBtn)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 5, 5)
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(operationPathLabel)
                        .addComponent(operationPath)
                        .addComponent(operationPathBtn)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 5, 5)
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(operationCmdLogLabel)
                        .addComponent(operationCmdLogPath)
                        .addComponent(operationCmdLogBtn)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 5, 5)
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(operOptionsPanel)
                )


                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 100, 100)
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(goButton)
                )
                .addGap(10)
        );

    }
}
