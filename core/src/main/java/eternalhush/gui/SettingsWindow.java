package eternalhush.gui;

import eternalhush.main.GlobalVariables;
import eternalhush.main.EternalHushWindow;

import javax.swing.*;
import java.awt.*;

public class SettingsWindow extends EternalHushWindow {
    private IconLoader iconLoader;

    private TabPanel verticalTab;

    private CustomizationPanel customizationPanel;
    private AutorunPanel generalTab;
    private InstalledListPanel installedListTab;
    private PluginsPanel pluginsPanel;

    public SettingsWindow() {
        super(WindowConstants.EMPTY_WINDOW, new BorderLayout());

        iconLoader = new IconLoader();
        verticalTab = new TabPanel();

        customizationPanel = new CustomizationPanel();
        generalTab = new AutorunPanel();
        installedListTab = new InstalledListPanel();
        pluginsPanel = new PluginsPanel();

        verticalTab.setTabPlacement(SwingConstants.LEFT);
        verticalTab.addTab("Customization", iconLoader.loadIcon("images/color.png"), customizationPanel);
        verticalTab.addTab("Products", iconLoader.loadIcon("images/installed.png", 32, 32), installedListTab);
        verticalTab.addTab("GUI Plugins", iconLoader.loadIcon("images/objects.png", 32, 32), pluginsPanel);
        verticalTab.addTab("Autorun", iconLoader.loadIcon("images/general.png", 32, 32), generalTab);

        add(verticalTab);
        GlobalVariables.rootFrame.pack();
    }
}
