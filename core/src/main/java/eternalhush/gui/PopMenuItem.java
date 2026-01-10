package eternalhush.gui;

import javax.swing.*;
import java.awt.*;

public class PopMenuItem extends JMenuItem {

    public PopMenuItem(String text) {
        super(text);
        this.setComponentOrientation(ComponentOrientation.LEFT_TO_RIGHT);
    }

    public PopMenuItem(String text, Icon i) {
        super(text, i);
        this.setComponentOrientation(ComponentOrientation.LEFT_TO_RIGHT);
    }
}
