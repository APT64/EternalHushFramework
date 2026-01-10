package eternalhush.gui;

import eternalhush.console.OperationConsole;
import eternalhush.main.GlobalVariables;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;

public class TabPanel extends JTabbedPane {
    Graphics graphics;
    BasePopupListener listener = e -> {
    };
    private boolean dragging = false;
    private Image tabImage = null;
    private Point currentMouseLocation = null;
    private int draggedTabIndex = 0;
    //private YPopupMenu popupMenu;
    private void detach(){
        TabPanel parentTabbedPane = TabPanel.this;
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
                    String tabTitle2 = ((OperationConsole) component).getHostname() + String.format(" [%d]", ((OperationConsole) component).getConsoleId());
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

    private void onTabOutsideFrame(int tab){
        this.setSelectedIndex(tab);
        detach();
    }

    public TabPanel() {
        setFocusable(false);
        setVisible(true);
 //       popupMenu = new YPopupMenu();
        this.addMouseListener(new MouseAdapter() {

            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()) {
                    for (int i = 0; i < TabPanel.this.getTabCount(); i++) {
                        Rectangle tab = getUI().getTabBounds(TabPanel.this, i);
                        if (tab.contains(e.getPoint())) {
                            // setSelectedIndex(i);
                            showPopupListener(e);
                        }
                    }
                }
            }

        });

        this.setPopupListener(new BasePopupListener() {
            @Override
            public void showPopup(MouseEvent e) {
                YPopupMenu popupMenu = new YPopupMenu();
                popupMenu.show((JComponent) e.getSource(), e.getX(), e.getY());
            }
        });
        addMouseMotionListener(new MouseMotionAdapter() {
            public void mouseDragged(MouseEvent e) {

                if (!dragging) {
                    // Gets the tab index based on the mouse position
                    int tabNumber = getUI().tabForCoordinate(TabPanel.this, e.getX(), e.getY());

                    if (tabNumber >= 0) {
                        draggedTabIndex = tabNumber;
                        Rectangle bounds = getUI().getTabBounds(TabPanel.this, tabNumber);


                        // Paint the tabbed pane to a buffer
                        Image totalImage = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_ARGB);
                        Graphics totalGraphics = totalImage.getGraphics();
                        totalGraphics.setClip(bounds);
                        // Don't be double buffered when painting to a static image.
                        setDoubleBuffered(false);
                        paintComponent(totalGraphics);

                        // Paint just the dragged tab to the buffer
                        tabImage = new BufferedImage(bounds.width, bounds.height, BufferedImage.TYPE_INT_ARGB);
                        graphics = tabImage.getGraphics();
                        graphics.drawImage(totalImage, 0, 0, bounds.width, bounds.height, bounds.x, bounds.y, bounds.x + bounds.width, bounds.y + bounds.height, TabPanel.this);

                        dragging = true;
                        repaint();
                    }
                } else {
                    currentMouseLocation = e.getPoint();

                    // Need to repaint
                    repaint();
                }

                super.mouseDragged(e);
            }
        });

        addMouseListener(new MouseAdapter() {
            public void mouseReleased(MouseEvent e) {

                if (dragging) {
                    if (!GlobalVariables.rootFrame.getRootPane().contains(e.getPoint())){
                        onTabOutsideFrame(draggedTabIndex);
                    }

                    int tabNumber = getUI().tabForCoordinate(TabPanel.this, e.getX(), e.getY());

                    if (tabNumber >= 0 && getTabCount() > 1) {
                        Component comp = getComponentAt(draggedTabIndex);

                        if (!getUI().getTabBounds(TabPanel.this, draggedTabIndex).getBounds().contains(e.getPoint())){
                            String title = getTitleAt(draggedTabIndex);
                            Icon icon = getIconAt(draggedTabIndex);
                            String tip = getToolTipTextAt(draggedTabIndex);
                            removeTabAt(draggedTabIndex);
                            insertTab(title, icon, comp, tip, tabNumber);
                            setSelectedComponent(comp);
                        }
                    }
                }
                if (tabImage != null) tabImage.flush();
                if (graphics != null) graphics.dispose();
                dragging = false;
                tabImage = null;
                repaint();
            }
        });
    }

    void showPopupListener(MouseEvent e) {
        listener.showPopup(e);
    }

    public void setPopupListener(BasePopupListener l) {
        listener = l;
    }

    protected void paintComponent(Graphics g) {
        super.paintComponent(g);

        // Are we dragging?
        if (dragging && currentMouseLocation != null && tabImage != null) {
            // Draw the dragged tab
            g.drawImage(tabImage, currentMouseLocation.x, currentMouseLocation.y, this);
        }
    }


}
