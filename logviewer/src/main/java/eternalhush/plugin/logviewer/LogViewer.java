package eternalhush.plugin.logviewer;

import eternalhush.plugin.BasePluginInterface;
import eternalhush.gui.PopMenuItem;
import eternalhush.gui.BasePopupListener;
import javax.swing.*;
import java.util.ArrayList;

public class LogViewer implements BasePluginInterface {
    LogViewerTabPanel panel;
    public LogViewer(){
        panel = new LogViewerTabPanel();
    }

    @Override
    public String getPluginName() {
        return "LogViewer";
    }

    @Override
    public String getPluginIcon() {
        return "images/search_text.png";
    }

    @Override
    public ArrayList<PopMenuItem> getPluginPopMenuEntryList() {
        return null;
    }

    @Override
    public JPanel getPluginTabPanel() {
        return panel;
    }

    @Override
    public Boolean isShowedByDefault(){ return false;}

    @Override
    public String getPluginVersion(){ return "1.0.0.0";}
}
