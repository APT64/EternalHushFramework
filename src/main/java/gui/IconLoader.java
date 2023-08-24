package gui;

import javax.swing.*;
import java.awt.*;
import java.net.URL;

public class IconLoader {
    public ImageIcon loadIcon(String name){
        URL _path = getClass().getResource("/"+name);

        if (_path != null) {
            ImageIcon icon = new ImageIcon(_path);
            return icon;
        }
        return null;
    }
    public ImageIcon loadIcon(String name, int x, int y){
        URL _path = getClass().getResource("/"+name);

        if (_path != null) {
            ImageIcon icon = new ImageIcon(_path);
            Image img = icon.getImage();
            Image scaled = img.getScaledInstance(x, y, Image.SCALE_SMOOTH);
            return new ImageIcon(scaled);
        }
        return null;
    }
}