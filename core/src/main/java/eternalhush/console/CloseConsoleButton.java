package eternalhush.console;

import eternalhush.gui.IconLoader;

import javax.swing.*;
import java.awt.*;

public class CloseConsoleButton extends JPanel {
    private Component tab;
    private IconLoader iconLoader;

    public CloseConsoleButton(final Component tab, String title, Icon icon) {
        this.tab = tab;
        iconLoader = new IconLoader();
        setOpaque(false);
        FlowLayout flowLayout = new FlowLayout(FlowLayout.CENTER, 3, 3);
        setLayout(flowLayout);
        JLabel jLabel = new JLabel(title);
        jLabel.setIcon(icon);
        add(jLabel);
        JButton button = new JButton(iconLoader.loadIcon("images/no.png", 16, 16));
        button.setContentAreaFilled(false);
        button.setMargin(new Insets(0, 0, 0, 0));
        button.addMouseListener(new CloseConsoleListener(tab));
        add(button);
    }
}