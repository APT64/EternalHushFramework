package eternalhush.gui;

import eternalhush.main.GlobalLogger;
import eternalhush.main.SettingsLoader;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;

public class LoadScreen extends JWindow {
    private JPanel contentPane;
    private JProgressBar bar;
    private ImagePanel imgPanel;
    private IconLoader iconLoader;
    private JLabel textLabel;
    private JPanel barPanel;

    public LoadScreen(String[] args) {
        setBounds(0, 0, 513, 331);
        setLocationRelativeTo(null);

        imgPanel = new ImagePanel("images/preloader.png");
        iconLoader = new IconLoader();
        contentPane = new JPanel();
        bar = new JProgressBar();
        bar.setSize(new Dimension(0, 14));
        bar.setPreferredSize(new Dimension(0, 14));
        bar.setMaximumSize(new Dimension(0, 14));

        barPanel = new JPanel();
        barPanel.setLayout(new BorderLayout());

        GlobalLogger.log("Preloader started");
        textLabel = new JLabel("Preloader started");
        textLabel.setFont(new Font(SettingsLoader.getKeyValue("console_font"), Font.PLAIN, 15));

        barPanel.add(textLabel, BorderLayout.PAGE_START);
        barPanel.add(bar, BorderLayout.PAGE_END);
        barPanel.setBorder(new EmptyBorder(10, 5, 2, 0));

        contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
        contentPane.setLayout(new BorderLayout());

        contentPane.add(barPanel, BorderLayout.PAGE_END);
        contentPane.add(new JLabel(iconLoader.loadIcon("images/preloader.png", 480, 300)));
        setContentPane(contentPane);

        setAlwaysOnTop(true);
        setVisible(true);

        try {
            Thread t1 = new Thread(new Runnable() {
                public void run() {
                    try {
                        GlobalLogger.log("Creating main window");
                        new MainWindow(args, new BasicFrame());
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            });
            t1.start();
        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    public void setLoadProgress(int pc) {
        bar.setValue(pc);
    }

    public void setLoadText(String text) {
        textLabel.setText(text);
    }

    public void updateStatus(String text, int pc) {
        setLoadProgress(pc);
        setLoadText(text);
        try {
            Thread.sleep(400);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

