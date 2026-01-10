package eternalhush.gui;

import eternalhush.main.GlobalVariables;
import eternalhush.plugin.BasePluginInterface;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;

public class PluginsPanel extends JPanel {
    public PluginsPanel() {
        setLayout(new BorderLayout());
        DefaultTableModel daDefaultTableModel = new DefaultTableModel(0, 0);

        // Column Names
        String[] columnNames = {"Plugin Name", "Version", "Showed By Default"};
        daDefaultTableModel.setColumnIdentifiers(columnNames);


        for (BasePluginInterface plugin : GlobalVariables.pluginList) {

            daDefaultTableModel.addRow(new Object[]{plugin.getPluginName(), plugin.getPluginVersion(), plugin.isShowedByDefault()});
        }

        JTable j = new JTable() {
            private static final long serialVersionUID = 1L;

            public boolean isCellEditable(int row, int column) {
                return false;
            }
        };
        j.setModel(daDefaultTableModel);
        j.setRowHeight(20);
        JScrollPane jScrollPane = new JScrollPane(j);
        add(jScrollPane, BorderLayout.CENTER);

    }

}
