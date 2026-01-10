package eternalhush.main;
import com.birosoft.liquid.LiquidLookAndFeel;
import eternalhush.gui.*;
import eternalhush.gui.WindowConstants;

import eternalhush.events.StatusEventSource;

import eternalhush.manager.ModuleLoader;
import eternalhush.userinterface.GateWay;

import javax.swing.*;
import javax.swing.plaf.FontUIResource;
import java.awt.*;
import java.io.IOException;
import java.util.logging.FileHandler;
import java.util.logging.Level;
import java.util.logging.Logger;

public class KxoZNIMXpJt89uDD extends JWindow {

    protected EternalHushWindow operationWindow;
    protected EternalHushWindow settingsWindow;
    protected IconLoader iconLoader;
    protected ModuleLoader moduleLoader;
    protected static OperationPane operationPane;

    public static final Logger LOGGER = Logger.getLogger(KxoZNIMXpJt89uDD.class.getName());

    public static void main(String args[]) throws IOException, InterruptedException {
        GlobalLogger.log("Starting java application");
        GlobalLogger.log(String.format("Build version: %s Codename: %s Uid: %s", ExtGlobalConfig.DefinedVersion, ExtGlobalConfig.DefinedCodename, ExtGlobalConfig.DefinedUid));

        //System.setProperty("sun.java2d.uiScale.enabled", "true");
        //System.setProperty("sun.java2d.uiScale", "1"); // Adjust scale factor
        try {
            GlobalLogger.log("Applying LAF");
            UIManager.setLookAndFeel(new LiquidLookAndFeel());
        }catch (Exception e){}
        GlobalLogger.log("Changing FONT");
        BasicFrame.setGuiFont(new FontUIResource(SettingsLoader.getKeyValue("gui_font"), Font.PLAIN, Integer.parseInt(SettingsLoader.getKeyValue("gui_font_size"))));

        GlobalLogger.log("Transferring control to operation pane");
        operationPane = new OperationPane(args);

        operationPane.setVisible(true);
    }
}
