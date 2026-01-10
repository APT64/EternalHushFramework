package eternalhush.plugin.remotefileviewer;
import eternalhush.console.CommandHandler;
import eternalhush.console.ConsoleManager;
import eternalhush.console.OperationConsole;
import eternalhush.main.GlobalVariables;
import eternalhush.gui.IconLoader;
import eternalhush.gui.NativeFileDialog;
import eternalhush.plugin.BasePluginInterface;
import eternalhush.gui.PopMenuItem;
import eternalhush.gui.BasePopupListener;
import eternalhush.gui.IconLoader;
import javax.swing.*;
import java.util.ArrayList;


public class RemoteFileViewer implements BasePluginInterface {
    private JPanel mainTabPanel;
    public RemoteFileViewer(){
        mainTabPanel = new RFWTabPanel();
    }
    @Override
    public String getPluginName() {
        return "FileViewer";
    }
    @Override
    public String getPluginIcon() {
        return "images/search_folder.png";
    }
    @Override
    public ArrayList<PopMenuItem> getPluginPopMenuEntryList() {
        return null;
    }
    @Override
    public JPanel getPluginTabPanel() {
        return mainTabPanel;
    }
    @Override
    public Boolean isShowedByDefault(){ return false;}

    @Override
    public String getPluginVersion(){ return "1.0.0.0";}
}
