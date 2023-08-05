package gui;

import eternalhush.GlobalVariables;
import eternalhush.SettingsLoader;

import javax.swing.*;

import java.awt.*;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;

import javax.swing.GroupLayout.*;
import javax.swing.LayoutStyle.*;
import javax.swing.border.EmptyBorder;
import javax.swing.plaf.FontUIResource;

public class CustomizationPanel extends JPanel {
    private JComboBox themeMenu;
    private JComboBox fontMenu;
    private JComboBox console_fontMenu;
    private IconLoader iconLoader;

    private JLabel themeLabel;
    private JLabel fontLabel;
    private JLabel console_fontLabel;
    public CustomizationPanel(){
        iconLoader = new IconLoader();

        fontLabel = new JLabel("GUI font: ");
        console_fontLabel = new JLabel("Console font: ");
        themeLabel = new JLabel("Theme: ");

        String themes[] = {"Mint", "Graphite", "Aluminium", "HiFi", "Noire"};

        themeMenu = new JComboBox(themes);
        fontMenu = new JComboBox(BasicFrame.getAllFontsNames().toArray());
        console_fontMenu = new JComboBox(BasicFrame.getAllFontsNames().toArray());

        themeMenu.setSelectedItem(SettingsLoader.getKeyValue("default_theme"));
        fontMenu.setSelectedItem(SettingsLoader.getKeyValue("gui_font"));
        console_fontMenu.setSelectedItem(SettingsLoader.getKeyValue("console_font"));

        themeMenu.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent itemEvent) {
                if (itemEvent.getStateChange() == ItemEvent.SELECTED){
                    BasicFrame.setupLookAndFeel(themeMenu.getSelectedItem().toString());

                    SettingsLoader.setKeyValue("default_theme", themeMenu.getSelectedItem().toString());
                    SwingUtilities.updateComponentTreeUI(GlobalVariables.rootFrame);
                    GlobalVariables.rootFrame.pack();
                }
            }
        });

        fontMenu.addItemListener(new ItemListener() {
            @Override
            public void itemStateChanged(ItemEvent itemEvent) {
                if (itemEvent.getStateChange() == ItemEvent.SELECTED){
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
                if (itemEvent.getStateChange() == ItemEvent.SELECTED){
                //    BasicFrame.setGuiFont(new FontUIResource(console_fontMenu.getSelectedItem().toString(), Font.PLAIN, Integer.valueOf(SettingsLoader.getKeyValue("gui_font_size"))));

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
                    .addComponent(themeLabel)
                    .addComponent(fontLabel)
                    .addComponent(console_fontLabel)
                )
                .addPreferredGap(ComponentPlacement.RELATED)

                .addGroup(layout.createParallelGroup()
                    .addComponent(themeMenu)
                    .addComponent(fontMenu)
                    .addComponent(console_fontMenu)
                )
                .addPreferredGap(ComponentPlacement.RELATED)
        );

        layout.setVerticalGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout
                        .createParallelGroup(Alignment.BASELINE)
                        .addComponent(themeLabel)
                        .addComponent(themeMenu)
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
        );

    }

}
