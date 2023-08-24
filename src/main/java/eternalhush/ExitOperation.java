package eternalhush;

import core.CoreConnector;
import gui.BasicFrame;

import javax.swing.*;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class ExitOperation extends WindowAdapter {
    @Override
    public void windowClosing(WindowEvent e) {
        BasicFrame currentFrame = (BasicFrame)e.getSource();
        if (JOptionPane.showConfirmDialog(currentFrame, "Close application?", "Application Exit", JOptionPane.OK_CANCEL_OPTION) == JOptionPane.OK_OPTION){
            //Очистка перед выходом

            //выход
            currentFrame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        }
    }
}
