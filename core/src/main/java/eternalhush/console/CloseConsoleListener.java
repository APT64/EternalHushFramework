package eternalhush.console;

import javax.swing.*;
import java.awt.*;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

public class CloseConsoleListener implements MouseListener {
    private Component tab;

    public CloseConsoleListener(Component tab) {
        this.tab = tab;
    }

    @Override
    public void mouseClicked(MouseEvent e) {
        if (e.getSource() instanceof JButton) {
            JButton clickedButton = (JButton) e.getSource();
            JTabbedPane tabbedPane = (JTabbedPane) clickedButton.getParent().getParent().getParent();
            if (tabbedPane.getTabCount() != 1) {
                tabbedPane.remove(tab);
            }
        }
    }

    @Override
    public void mousePressed(MouseEvent e) {
    }

    @Override
    public void mouseReleased(MouseEvent e) {
    }

    @Override
    public void mouseEntered(MouseEvent e) {
    }

    @Override
    public void mouseExited(MouseEvent e) {
    }
}