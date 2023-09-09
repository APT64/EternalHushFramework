package clingyspider;

import console.CommandHandler;
import console.ConsoleManager;
import console.OperationConsole;
import eternalhush.GlobalVariables;
import gui.IconLoader;
import gui.NativeFileDialog;
import gui.TabPanel;

import javax.swing.*;
import javax.swing.filechooser.FileNameExtensionFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

public class ClingySpider extends JPanel {
    private JButton runBtn;
    private JRadioButton typeListen;
    private JRadioButton typeConnect;
    private JLabel privateKey;
    private JComboBox keyVariant;
    private JButton browse;
    private JPanel targetOptions;
    private JPanel implantOptions;
    private JLabel implantAddrLabel;
    private JTextField implantAddr;
    private JLabel implantPortLabel;
    private JTextField implantPort;
    private ButtonGroup connectionType;

    private File keyPath;

    public ClingySpider(){
        connectionType = new ButtonGroup();
        runBtn = new JButton("Connect to Implant");
        typeConnect = new JRadioButton("Connect");
        typeConnect.setSelected(true);
        typeListen = new JRadioButton("Listen");
        privateKey = new JLabel("Private key:");
        keyVariant = new JComboBox();
        browse = new JButton("Browse Key File");
        implantAddrLabel = new JLabel("Implant Host");
        implantPortLabel = new JLabel("Implant Port");
        implantPort = new JTextField("1287");
        implantAddr = new JTextField();
        targetOptions = new JPanel();
        implantOptions = new JPanel();

        typeConnect.setFocusPainted(false);
        typeListen.setFocusPainted(false);

        connectionType.add(typeConnect);
        connectionType.add(typeListen);
        targetOptions.setBorder(BorderFactory.createTitledBorder("Connection Details"));

        typeConnect.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                targetOptions.setVisible(true);
            }
        });
        typeListen.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                targetOptions.setVisible(false);
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
                    GlobalVariables.operationPanel.addTab(consoleInstance.getHostname()+ " ["+consoleInstance.getConsoleId()+"]", new IconLoader().loadIcon("images/console_icon.png", 31, 31), consoleInstance);
                }
                if (keyVariant.getSelectedItem() == null){
                   keyVariant.setBackground(Color.RED);
                   return;
                }
                if (implantPort.getText().equalsIgnoreCase("")){
                    implantPort.setBackground(Color.RED);
                   return;
                }
                if (implantAddr.getText().equalsIgnoreCase("")){
                    implantAddr.setBackground(Color.RED);
                   return;
                }
                GlobalVariables.rootTabPanel.setSelectedIndex(0);
                GlobalVariables.operationPanel.setSelectedIndex(consoleInstance.getConsoleId());
                CommandHandler handler = new CommandHandler();
                handler.CommonHandler("!clsp_connect " + implantAddr.getText() + " " + implantPort.getText() + " " + keyVariant.getSelectedItem(), consoleInstance);
            }
        });

        GroupLayout connectionDetails = new GroupLayout(targetOptions);
        targetOptions.setLayout(connectionDetails);
        GroupLayout implantLayout = new GroupLayout(implantOptions);
        implantOptions.setLayout(implantLayout);
        GroupLayout layout = new GroupLayout(this);
        setLayout(layout);


        connectionDetails.setHorizontalGroup(connectionDetails.createSequentialGroup()
                .addGroup(connectionDetails.createParallelGroup()
                        .addComponent(implantAddrLabel)
                        .addComponent(implantPortLabel)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(connectionDetails.createParallelGroup()
                        .addComponent(implantAddr, 10, 130, 130)
                        .addComponent(implantPort, 10, 10, 50)
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

        );
    }
}
