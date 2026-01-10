package eternalhush.gui;

import eternalhush.main.GlobalVariables;
import eternalhush.main.SettingsLoader;

import javax.swing.*;
import javax.swing.GroupLayout.Alignment;
import javax.swing.LayoutStyle.ComponentPlacement;
import javax.swing.plaf.FontUIResource;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

public class CustomizationPanel extends JPanel {
    private JComboBox fontMenu;
    private JComboBox console_fontMenu;
    private JButton goodColor;
    private JButton warnColor;
    private JButton errorColor;
    private JButton defaultColor;
    private IconLoader iconLoader;

    private JLabel fontLabel;
    private JLabel console_fontLabel;
    private JLabel goodLabel;
    private JLabel errorLabel;
    private JLabel warnLabel;
    private JLabel defaultLabel;

    private JTextField exampleGood;
    private JTextField exampleError;
    private JTextField exampleWarn;
    private JTextField exampleDefault;

    public CustomizationPanel() {
        iconLoader = new IconLoader();

        goodColor = new JButton("Change");
        goodColor.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                Color newColor = JColorChooser.showDialog(
                        CustomizationPanel.this,
                        "Choose Color",
                        Color.decode(SettingsLoader.getKeyValue("good_color")));
                if (newColor == null) {
                    return;
                }
                SettingsLoader.setKeyValue("good_color", String.format("#%06x", newColor.getRGB() & 0xFFFFFF));
                exampleGood.setForeground(newColor);
            }
        });
        warnColor = new JButton("Change");
        warnColor.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                Color newColor = JColorChooser.showDialog(
                        CustomizationPanel.this,
                        "Choose Color",
                        Color.decode(SettingsLoader.getKeyValue("warn_color")));
                if (newColor == null) {
                    return;
                }
                SettingsLoader.setKeyValue("warn_color", String.format("#%06x", newColor.getRGB() & 0xFFFFFF));
                exampleWarn.setForeground(newColor);
            }
        });
        errorColor = new JButton("Change");
        errorColor.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                Color newColor = JColorChooser.showDialog(
                        CustomizationPanel.this,
                        "Choose Color",
                        Color.decode(SettingsLoader.getKeyValue("error_color")));
                if (newColor == null) {
                    return;
                }
                SettingsLoader.setKeyValue("error_color", String.format("#%06x", newColor.getRGB() & 0xFFFFFF));
                exampleError.setForeground(newColor);
            }
        });
        defaultColor = new JButton("Change");
        defaultColor.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                Color newColor = JColorChooser.showDialog(
                        CustomizationPanel.this,
                        "Choose Color",
                        Color.decode(SettingsLoader.getKeyValue("default_color")));
                if (newColor == null) {
                    return;
                }
                SettingsLoader.setKeyValue("default_color", String.format("#%06x", newColor.getRGB() & 0xFFFFFF));
                exampleDefault.setForeground(newColor);
            }
        });


        fontLabel = new JLabel("GUI font: ");
        console_fontLabel = new JLabel("Console font: ");

        goodLabel = new JLabel("Good text: ");
        errorLabel = new JLabel("Error text: ");
        warnLabel = new JLabel("Warning text: ");
        defaultLabel = new JLabel("Default text: ");

        exampleGood = new JTextField("This is good text");
        exampleGood.setBackground(Color.BLACK);
        exampleGood.setForeground(Color.decode(SettingsLoader.getKeyValue("good_color")));

        exampleError = new JTextField("This is error text");
        exampleError.setBackground(Color.BLACK);
        exampleError.setForeground(Color.decode(SettingsLoader.getKeyValue("error_color")));

        exampleWarn = new JTextField("This is warning text");
        exampleWarn.setBackground(Color.BLACK);
        exampleWarn.setForeground(Color.decode(SettingsLoader.getKeyValue("warn_color")));

        exampleDefault = new JTextField("This is default text");
        exampleDefault.setBackground(Color.BLACK);
        exampleDefault.setForeground(Color.decode(SettingsLoader.getKeyValue("default_color")));

        fontMenu = new JComboBox(BasicFrame.getAllFontsNames().toArray());
        console_fontMenu = new JComboBox(BasicFrame.getAllFontsNames().toArray());

        fontMenu.setSelectedItem(SettingsLoader.getKeyValue("gui_font"));
        console_fontMenu.setSelectedItem(SettingsLoader.getKeyValue("console_font"));

        fontMenu.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent itemEvent) {
                if (itemEvent.getStateChange() == ItemEvent.SELECTED) {
                    BasicFrame.setGuiFont(new FontUIResource(fontMenu.getSelectedItem().toString(), Font.PLAIN, Integer.valueOf(SettingsLoader.getKeyValue("gui_font_size"))));

                    SettingsLoader.setKeyValue("gui_font", fontMenu.getSelectedItem().toString());
                    SwingUtilities.updateComponentTreeUI(GlobalVariables.rootFrame);
                    GlobalVariables.rootFrame.pack();
                }
            }
        });
        console_fontMenu.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent itemEvent) {
                if (itemEvent.getStateChange() == ItemEvent.SELECTED) {
                    SettingsLoader.setKeyValue("console_font", console_fontMenu.getSelectedItem().toString());
                    SwingUtilities.updateComponentTreeUI(GlobalVariables.rootFrame);
                    GlobalVariables.rootFrame.pack();
                }
            }
        });


        GroupLayout layout = new GroupLayout(this);
        setLayout(layout);

        layout.setHorizontalGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup()
                        .addComponent(fontLabel)
                        .addComponent(console_fontLabel)
                        .addComponent(goodLabel)
                        .addComponent(errorLabel)
                        .addComponent(warnLabel)
                        .addComponent(defaultLabel)
                )
                .addPreferredGap(ComponentPlacement.RELATED)

                .addGroup(layout.createParallelGroup()
                        .addComponent(fontMenu)
                        .addComponent(console_fontMenu)
                        .addComponent(exampleGood)
                        .addComponent(exampleError)
                        .addComponent(exampleWarn)
                        .addComponent(exampleDefault)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup()
                        .addComponent(goodColor)
                        .addComponent(errorColor)
                        .addComponent(warnColor)
                        .addComponent(defaultColor)

                )
                .addContainerGap()
        );

        layout.setVerticalGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                        .addComponent(fontLabel)
                        .addComponent(fontMenu)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                        .addComponent(console_fontLabel)
                        .addComponent(console_fontMenu)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                        .addComponent(goodLabel)
                        .addComponent(exampleGood)
                        .addComponent(goodColor)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                        .addComponent(errorLabel)
                        .addComponent(exampleError)
                        .addComponent(errorColor)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                        .addComponent(warnLabel)
                        .addComponent(exampleWarn)
                        .addComponent(warnColor)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                        .addComponent(defaultLabel)
                        .addComponent(exampleDefault)
                        .addComponent(defaultColor)
                )

        );

    }

}
