package eternalhush;

import gui.BasicFrame;
import manager.CommonModule;

import java.awt.*;
import java.util.ArrayList;

public abstract class GlobalVariables {
    public static final String VERSION = "1.0.0.0";
    public static ArrayList<CommonModule> commonModuleList = new ArrayList<>();
    public static BasicFrame rootFrame;
    public static Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

}
