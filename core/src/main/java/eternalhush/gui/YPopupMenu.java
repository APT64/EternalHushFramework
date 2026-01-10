package eternalhush.gui;

import eternalhush.console.OperationConsole;
import eternalhush.main.GlobalVariables;
import eternalhush.plugin.BasePluginInterface;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

public class YPopupMenu extends JPopupMenu {
    IconLoader iconLoader;
    JMenu tabsMenu;
    JMenu pluginsMenu;
    public PopMenuItem hideTab;
    public PopMenuItem detachTab;
    PopMenuItem tabsTop;
    PopMenuItem tabsRight;
    PopMenuItem tabsLeft;
    PopMenuItem tabsBottom;

    public YPopupMenu() {
        super();
        iconLoader = new IconLoader();
        tabsMenu = new JMenu("Align Tabs");
        detachTab = new PopMenuItem("Detach", iconLoader.loadIcon("images/detach.png", 17, 17));
        pluginsMenu = new JMenu("Plugins");
        pluginsMenu.setIcon(iconLoader.loadIcon("images/objects.png", 17, 17));
        hideTab = new PopMenuItem("Hide", iconLoader.loadIcon("images/hide.png", 17, 17));
        tabsTop = new PopMenuItem("Top");
        tabsRight = new PopMenuItem("Right");
        tabsLeft = new PopMenuItem("Left");
        tabsBottom = new PopMenuItem("Bottom");

        tabsTop.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                JPopupMenu parentMenu = (JPopupMenu) ((PopMenuItem) actionEvent.getSource()).getParent();
                YPopupMenu parentPopMenu = (YPopupMenu) findComponentAt(parentMenu.getLocation());
                TabPanel parentTabbedPane = (TabPanel) parentPopMenu.getInvoker();
                parentTabbedPane.setTabPlacement(SwingConstants.TOP);
            }
        });
        tabsRight.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                JPopupMenu parentMenu = (JPopupMenu) ((PopMenuItem) actionEvent.getSource()).getParent();
                YPopupMenu parentPopMenu = (YPopupMenu) findComponentAt(parentMenu.getLocation());
                TabPanel parentTabbedPane = (TabPanel) parentPopMenu.getInvoker();
                parentTabbedPane.setTabPlacement(SwingConstants.RIGHT);
            }
        });
        tabsLeft.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                JPopupMenu parentMenu = (JPopupMenu) ((PopMenuItem) actionEvent.getSource()).getParent();
                YPopupMenu parentPopMenu = (YPopupMenu) findComponentAt(parentMenu.getLocation());
                TabPanel parentTabbedPane = (TabPanel) parentPopMenu.getInvoker();
                parentTabbedPane.setTabPlacement(SwingConstants.LEFT);
            }
        });
        tabsBottom.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                JPopupMenu parentMenu = (JPopupMenu) ((PopMenuItem) actionEvent.getSource()).getParent();
                YPopupMenu parentPopMenu = (YPopupMenu) findComponentAt(parentMenu.getLocation());
                TabPanel parentTabbedPane = (TabPanel) parentPopMenu.getInvoker();
                parentTabbedPane.setTabPlacement(SwingConstants.BOTTOM);
            }
        });
        hideTab.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                JPopupMenu parentMenu = (JPopupMenu) ((PopMenuItem) actionEvent.getSource()).getParent();
                YPopupMenu parentPopMenu = (YPopupMenu) findComponentAt(parentMenu.getLocation());
                TabPanel parentTabbedPane = (TabPanel) parentPopMenu.getInvoker();
                int selected_idx = parentTabbedPane.getSelectedIndex();
                parentTabbedPane.removeTabAt(selected_idx);
            }
        });
        detachTab.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                try {
                    JPopupMenu parentMenu = (JPopupMenu) ((PopMenuItem) actionEvent.getSource()).getParent();
                    YPopupMenu parentPopMenu = (YPopupMenu) findComponentAt(parentMenu.getLocation());
                    TabPanel parentTabbedPane = (TabPanel) parentPopMenu.getInvoker();
                    int selected_idx = parentTabbedPane.getSelectedIndex();
                    String tabTitle = parentTabbedPane.getTitleAt(selected_idx);
                    String tabTip = parentTabbedPane.getToolTipTextAt(selected_idx);
                    Icon tabIcon = parentTabbedPane.getIconAt(selected_idx);

                    Component component = parentTabbedPane.getComponentAt(selected_idx);
                    JFrame frame = new JFrame();
                    frame.setTitle(tabTitle);
                    frame.addWindowListener(new WindowAdapter() {
                        public void windowClosing(WindowEvent e) {
                            if (component instanceof OperationConsole) {
                                ((OperationConsole) component).setDetached(false);
                                String tabTitle2 = ((OperationConsole) component).getHostname();
                                parentTabbedPane.insertTab(tabTitle2, tabIcon, component, tabTip, selected_idx);
                            } else {
                                parentTabbedPane.insertTab(tabTitle, tabIcon, component, tabTip, selected_idx);
                            }
                            parentTabbedPane.setSelectedIndex(selected_idx);
                        }
                    });
                    if (component instanceof OperationConsole) {
                        ((OperationConsole) component).setDetached(true);
                    }
                    frame.setLayout(new BorderLayout());
                    frame.setSize(component.getSize());
                    frame.add(component, BorderLayout.CENTER);
                    frame.setVisible(true);
                }
                catch (Exception e){
                    e.printStackTrace();
                }
            }
        });

        add(tabsMenu);

        tabsMenu.add(tabsTop);
        tabsMenu.add(tabsRight);
        tabsMenu.add(tabsLeft);
        tabsMenu.add(tabsBottom);

        add(detachTab);
        add(hideTab);

        add(pluginsMenu);
        for (BasePluginInterface plugin : GlobalVariables.pluginList) {
            String iconPath = plugin.getPluginIcon();
            PopMenuItem item = new PopMenuItem(plugin.getPluginName(), iconLoader.loadIcon(iconPath, plugin.getClass(), 17, 17));
            System.out.println(item);
            item.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent actionEvent) {
                    GlobalVariables.rootTabPanel.addTab(plugin.getPluginName(), iconLoader.loadIcon(iconPath, plugin.getClass(), 32, 32), plugin.getPluginTabPanel());
                }
            });
            pluginsMenu.add(item);
        }
    }
}
