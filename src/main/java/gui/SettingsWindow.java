package gui;

import eternalhush.EternalHushWindow;
import eternalhush.GlobalVariables;

import javax.swing.*;
import java.awt.*;

public class SettingsWindow extends EternalHushWindow {
    private IconLoader iconLoader;

    private TabPanel verticalTab;

    private CustomizationPanel customizationPanel;
    private EternalHushWindow generalTab;

    public SettingsWindow(){
        super(WindowConstants.EMPTY_WINDOW, new BorderLayout());

        iconLoader = new IconLoader();
        verticalTab = new TabPanel();

        customizationPanel = new CustomizationPanel();
        generalTab = new EternalHushWindow(WindowConstants.EMPTY_WINDOW);

        verticalTab.setTabPlacement(SwingConstants.LEFT);
        verticalTab.addTab("General", iconLoader.loadIcon("images/settings2_icon.png"), generalTab);
        verticalTab.addTab("Customization", iconLoader.loadIcon("images/color_icon.png"), customizationPanel);

        add(verticalTab);
        GlobalVariables.rootFrame.pack();
    }
}
