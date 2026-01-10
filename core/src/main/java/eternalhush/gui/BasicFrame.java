package eternalhush.gui;

import eternalhush.main.ExitOperation;

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

    public static ArrayList<String> getAllFontsNames() {
        ArrayList<String> allFonts = new ArrayList<>();

        GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
        Font[] fonts = ge.getAllFonts();

        for (Font font : fonts) {
            allFonts.add(font.getName());
        }
        return allFonts;
    }

    public static void setGuiFont(FontUIResource font) {
        java.util.Enumeration keys = UIManager.getLookAndFeelDefaults().keys();
        while (keys.hasMoreElements()) {
            Object key = keys.nextElement();
            Object val = UIManager.getLookAndFeelDefaults().get(key);
            if (val instanceof Font || val instanceof FontUIResource) {
                UIManager.getLookAndFeelDefaults().put(key, font);
            }
        }
    }

    @Override
    public void pack() {
        Dimension appSize = getSize();
        Point appLocation = getLocation();
        super.pack();
        setSize(appSize);
        setLocation(appLocation);
    }
}
