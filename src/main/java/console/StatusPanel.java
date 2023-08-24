package console;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import gui.IconLoader;

public class StatusPanel extends JPanel{
    IconLoader iconLoader;
    private JTextField userInputField;
    private JButton btnYes;
    private JButton btnNo;
    private JPanel buttonContainer;


    public StatusPanel() {
        iconLoader = new IconLoader();
        userInputField = new JTextField();
        buttonContainer = new JPanel();

        setLayout(new BorderLayout());

        btnNo = new JButton(null, iconLoader.loadIcon("images/cancel_icon.png", 20, 20));
        btnYes = new JButton(null, iconLoader.loadIcon("images/accept_icon.png", 20, 20));

        btnYes.setContentAreaFilled(false);
        btnYes.setFocusPainted(false);
        btnYes.setBorder(BorderFactory.createEmptyBorder(0,5,0,5));

        btnNo.setContentAreaFilled(false);
        btnNo.setFocusPainted(false);
        btnNo.setBorder(BorderFactory.createEmptyBorder(0,5,0,5));

        btnYes.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), "Do you love god?");
            }
        });
        btnNo.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                JOptionPane.showConfirmDialog(JOptionPane.getRootFrame(), "Do you love god?");
            }
        });

        buttonContainer.add(btnYes);
        buttonContainer.add(btnNo);

        add(userInputField, BorderLayout.CENTER);
        add(buttonContainer, BorderLayout.EAST);
    }

    public void setGlobalEnabled(boolean flag){
        btnYes.setEnabled(flag);
        btnNo.setEnabled(flag);
        userInputField.setEnabled(flag);
    }
}
