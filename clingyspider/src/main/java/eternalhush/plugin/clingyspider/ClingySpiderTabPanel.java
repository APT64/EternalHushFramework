package eternalhush.plugin.clingyspider;
import eternalhush.console.CommandHandler;
import eternalhush.console.ConsoleManager;
import eternalhush.console.OperationConsole;
import eternalhush.main.GlobalVariables;
import eternalhush.gui.IconLoader;
import eternalhush.gui.NativeFileDialog;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

public class ClingySpiderTabPanel extends JPanel {
    private JButton runBtn;
    private JRadioButton typeListen;
    private JRadioButton typeConnect;
    private JLabel privateKey;
    private JComboBox keyVariant;
    private JButton browse;
    private JPanel targetOptions;
    private JPanel listenOptions;
    private JPanel implantOptions;
    private JLabel implantAddrLabel;
    private JLabel listenPortLabel;
    private JTextField implantAddr;
    private JLabel implantPortLabel;
    private JTextField implantPort;
    private JTextField listenPort;
    private ButtonGroup connectionType;
    private JCheckBox connectBackground;
    private JRadioButton connectTcp;
    private JCheckBox listenBackground;
    private JRadioButton listenHttp;

    private JCheckBox listenRestart;
    private JCheckBox connectRestart;
    private JLabel listenRestartLabel;
    private JLabel connectRestartLabel;

    private JLabel implantBackground;
    private JLabel implantConnectionType;
    private JLabel listenBackgroundLabel;
    private JLabel listenProtoType;


    private File keyPath;

    public ClingySpiderTabPanel(){
        connectionType = new ButtonGroup();

        implantConnectionType = new JLabel("Protocol: ");
        implantBackground = new JLabel("Background: ");

        listenProtoType = new JLabel("Protocol: ");
        listenBackgroundLabel = new JLabel("Background: ");

        runBtn = new JButton("Connect to Implant");
        typeConnect = new JRadioButton("Connect");
        typeConnect.setSelected(true);
        typeListen = new JRadioButton("Listen");
        privateKey = new JLabel("Private key:");
        keyVariant = new JComboBox();
        browse = new JButton("Browse Key File");
        implantAddrLabel = new JLabel("Implant Host:");
        implantPortLabel = new JLabel("Implant Port:");
        listenPortLabel = new JLabel("Listener Port:");
        implantPort = new JTextField("1287");
        listenPort = new JTextField("1287");
        implantAddr = new JTextField();
        targetOptions = new JPanel();
        listenOptions = new JPanel();
        implantOptions = new JPanel();

        listenRestart = new JCheckBox();
        listenRestart.setSelected(true);
        connectRestart = new JCheckBox();
        connectRestart.setSelected(true);
        listenRestartLabel = new JLabel("Autorun task:");
        connectRestartLabel = new JLabel("Autorun task:");

        connectBackground = new JCheckBox();
        connectBackground.setSelected(false);
        connectTcp = new JRadioButton("TCP");
        connectTcp.setSelected(true);
        listenBackground = new JCheckBox();
        listenBackground.setSelected(true);
        listenHttp = new JRadioButton("HTTP");
        listenHttp.setSelected(true);

        typeConnect.setFocusPainted(false);
        typeListen.setFocusPainted(false);

        connectionType.add(typeConnect);
        connectionType.add(typeListen);
        targetOptions.setBorder(BorderFactory.createTitledBorder("Connection Details"));
        listenOptions.setBorder(BorderFactory.createTitledBorder("Listener Details"));

        listenOptions.setVisible(false);
        typeConnect.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                targetOptions.setVisible(true);
                listenOptions.setVisible(false);
                runBtn.setText("Connect to Implant");
            }
        });
        typeListen.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                targetOptions.setVisible(false);
                listenOptions.setVisible(true);
                runBtn.setText("Start Listening");
            }
        });

        browse.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                NativeFileDialog fileChooser = new NativeFileDialog();
                fileChooser.setFileFilter(new FileNameExtensionFilter("Key file", "*.*"));
                fileChooser.showDialog(keyVariant,"Open");
                keyPath = fileChooser.getSelectedFile();
                keyVariant.addItem(keyPath);
            }
        });
        runBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                OperationConsole consoleInstance = ConsoleManager.getFirstDefault();
                if (consoleInstance == null){
                    consoleInstance = new OperationConsole();
                    GlobalVariables.operationPanel.addTab(consoleInstance.getHostname()+ " ["+consoleInstance.getConsoleId()+"]", new IconLoader().loadIcon("images/tv.png", 32, 32), consoleInstance);
                }
                if (keyVariant.getSelectedItem() == null){
                    keyVariant.setBackground(Color.RED);
                    return;
                }
                if (typeConnect.isSelected()){
                    if (implantPort.getText().equalsIgnoreCase("")){
                        implantPort.setBackground(Color.RED);
                        return;
                    }
                    if (implantAddr.getText().equalsIgnoreCase("")){
                        implantAddr.setBackground(Color.RED);
                        return;
                    }
                }
                if (typeListen.isSelected()){
                    if (listenPort.getText().equalsIgnoreCase("")){
                        listenPort.setBackground(Color.RED);
                        return;
                    }
                }
                GlobalVariables.rootTabPanel.setSelectedIndex(0);
                if ( consoleInstance.getDetached()) {
                    SwingUtilities.windowForComponent(consoleInstance).toFront();
                    SwingUtilities.windowForComponent(consoleInstance).requestFocus();
                }else {
                    GlobalVariables.operationPanel.setSelectedIndex(consoleInstance.getConsoleTabId());
                }
                CommandHandler handler = new CommandHandler();
                String background = "";
                String autorun = "";
                if (typeConnect.isSelected()) {
                    if (connectRestart.isSelected()){
                        autorun = " -restart -force";
                    }
                    if (connectBackground.isSelected()){
                        background = " -background";
                    }
                    handler.CommonHandler("!cs_connect -relock -host " + implantAddr.getText() + " -port " + implantPort.getText() + " -key " + keyVariant.getSelectedItem() + background + autorun, consoleInstance, 1);
                }
                if (typeListen.isSelected()){
                    if (listenRestart.isSelected()){
                        autorun = " -restart -force";
                    }
                    if (listenBackground.isSelected()){
                        background = " -background";
                    }
                    handler.CommonHandler("!cs_listen -port " + listenPort.getText() + " -key " + keyVariant.getSelectedItem()+background+autorun, consoleInstance, 1);
                }
            }
        });

        GroupLayout connectionDetails = new GroupLayout(targetOptions);
        GroupLayout listenDetails = new GroupLayout(listenOptions);
        targetOptions.setLayout(connectionDetails);
        listenOptions.setLayout(listenDetails);
        GroupLayout implantLayout = new GroupLayout(implantOptions);
        implantOptions.setLayout(implantLayout);
        GroupLayout layout = new GroupLayout(this);
        setLayout(layout);

        listenDetails.setHorizontalGroup(listenDetails.createSequentialGroup()
                .addGroup(listenDetails.createParallelGroup()
                        .addComponent(listenPortLabel)
                        .addComponent(listenProtoType)
                        .addComponent(listenBackgroundLabel)
                        .addComponent(listenRestartLabel)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(listenDetails.createParallelGroup()
                        .addComponent(listenPort, 10, 70, 70)
                        .addComponent(listenHttp)
                        .addComponent(listenBackground)
                        .addComponent(listenRestart)
                )
                .addContainerGap(100, 30000)
        );

        listenDetails.setVerticalGroup(listenDetails.createSequentialGroup()
                .addGroup(listenDetails
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(listenPortLabel)
                        .addComponent(listenPort)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(listenDetails.createParallelGroup()
                        .addComponent(listenProtoType)
                        .addComponent(listenHttp)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(listenDetails.createParallelGroup()
                        .addComponent(listenBackgroundLabel)
                        .addComponent(listenBackground)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(listenDetails.createParallelGroup()
                        .addComponent(listenRestartLabel)
                        .addComponent(listenRestart)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 5, 5)
        );

        connectionDetails.setHorizontalGroup(connectionDetails.createSequentialGroup()
                .addGroup(connectionDetails.createParallelGroup()
                        .addComponent(implantAddrLabel)
                        .addComponent(implantPortLabel)
                        .addComponent(implantConnectionType)
                        .addComponent(implantBackground)
                        .addComponent(connectRestartLabel)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(connectionDetails.createParallelGroup()
                        .addComponent(implantAddr, 10, 150, 150)
                        .addComponent(implantPort, 10, 70, 70)
                        .addComponent(connectTcp)
                        .addComponent(connectBackground)
                        .addComponent(connectRestart)
                )
                .addContainerGap(100, 30000)
        );

        connectionDetails.setVerticalGroup(connectionDetails.createSequentialGroup()
                .addGroup(connectionDetails
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(implantAddrLabel)
                        .addComponent(implantAddr)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(connectionDetails
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(implantPortLabel)
                        .addComponent(implantPort)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(connectionDetails
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(implantConnectionType)
                        .addComponent(connectTcp)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(connectionDetails
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(implantBackground)
                        .addComponent(connectBackground)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(connectionDetails
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(connectRestartLabel)
                        .addComponent(connectRestart)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED, 5, 5)
        );


        implantLayout.setHorizontalGroup(implantLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(implantLayout.createParallelGroup()
                        .addComponent(typeConnect)
                        .addComponent(privateKey)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(implantLayout.createParallelGroup()
                        .addComponent(typeListen)
                        .addComponent(keyVariant, 10, 5000, 30000)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(implantLayout.createParallelGroup()
                        .addComponent(runBtn)
                        .addComponent(browse)
                )
                .addContainerGap()
        );

        implantLayout.setVerticalGroup(implantLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(implantLayout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(typeConnect)
                        .addComponent(typeListen)
                        .addComponent(runBtn)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(implantLayout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(privateKey)
                        .addComponent(keyVariant)
                        .addComponent(browse)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(implantLayout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(privateKey)
                        .addComponent(keyVariant)
                        .addComponent(browse)
                )
        );

        layout.setHorizontalGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup()
                        .addComponent(implantOptions)
                        .addComponent(targetOptions)
                        .addComponent(listenOptions)
                )
                .addContainerGap()
        );

        layout.setVerticalGroup(layout.createSequentialGroup()
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(implantOptions)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(targetOptions)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(listenOptions)
                )

        );
    }
}
