package gui;
import com.jtattoo.plaf.graphite.GraphiteLookAndFeel;
import com.jtattoo.plaf.mint.MintLookAndFeel;
import com.jtattoo.plaf.aluminium.AluminiumLookAndFeel;
import com.jtattoo.plaf.hifi.HiFiLookAndFeel;
import com.jtattoo.plaf.noire.NoireLookAndFeel;
import eternalhush.ExitOperation;
import eternalhush.GlobalVariables;

import javax.swing.*;
import javax.swing.plaf.FontUIResource;
import java.awt.*;
import java.util.ArrayList;

public class BasicFrame extends JFrame {

    public BasicFrame() {

        super("");
        setLayout(new BorderLayout());

        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        setExtendedState(JFrame.NORMAL);

//        KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(this);

        this.addWindowListener(new ExitOperation());
    }
    @Override
    public void pack(){
        Dimension appSize = getSize();
        Point appLocation = getLocation();
        super.pack();
        setSize(appSize);
        setLocation(appLocation);
    }


    public static void setupLookAndFeel(String style) {
        try {

            if (style.equals("Graphite")) {
                UIManager.setLookAndFeel(new GraphiteLookAndFeel());
            }
            else if (style.equals("Mint")) {
                UIManager.setLookAndFeel(new MintLookAndFeel());
            }
            else if (style.equals("Aluminium")) {
                UIManager.setLookAndFeel(new AluminiumLookAndFeel());
            }
            else if (style.equals("HiFi")) {
                UIManager.setLookAndFeel(new HiFiLookAndFeel());
            }
            else if (style.equals("Noire")) {
                UIManager.setLookAndFeel(new NoireLookAndFeel());
            }
            else {
                for (UIManager.LookAndFeelInfo info : UIManager.getInstalledLookAndFeels()) {
                    if (style.equals(info.getName())) {
                        UIManager.setLookAndFeel(info.getClassName());
                    }
                }
            }

        } catch (Exception e) {
        }
    }

    public static ArrayList<String> getAllFontsNames(){
        ArrayList<String> allFonts = new ArrayList<>();

        GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();;
        Font fonts[] = ge.getAllFonts();

        for(Font font : fonts){
            allFonts.add(font.getName());
        }
        return allFonts;
    }

    public static void setGuiFont(FontUIResource font){
        java.util.Enumeration keys = UIManager.getLookAndFeelDefaults().keys();
        while (keys.hasMoreElements()){
            Object key = keys.nextElement();
            Object val = UIManager.getLookAndFeelDefaults().get(key);
            if(val instanceof Font || val instanceof FontUIResource){
                UIManager.getLookAndFeelDefaults().put(key, font);
            }
        }
    }
}
