package eternalhush.plugin;

import eternalhush.gui.PopMenuItem;

import javax.swing.*;
import java.util.ArrayList;

public interface BasePluginInterface {
    String getPluginName();

    String getPluginIcon();

    ArrayList<PopMenuItem> getPluginPopMenuEntryList();

    JPanel getPluginTabPanel();

    Boolean isShowedByDefault();

    String getPluginVersion();
}
