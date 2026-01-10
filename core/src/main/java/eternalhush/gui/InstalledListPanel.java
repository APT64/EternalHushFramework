package eternalhush.gui;

import eternalhush.main.GlobalVariables;
import eternalhush.manager.MetaInfo;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;
import java.awt.*;

public class InstalledListPanel extends JPanel {
    public InstalledListPanel() {
        setLayout(new BorderLayout());
        DefaultTableModel daDefaultTableModel = new DefaultTableModel(0, 0);

        // Column Names
        String[] columnNames = {"Product", "Version", "Module"};
        daDefaultTableModel.setColumnIdentifiers(columnNames);


        for (MetaInfo info : GlobalVariables.metaModuleList) {
            String ver = String.format("%d.%d.%d.%d", info.getMajor(), info.getMinor(), info.getFix(), info.getBuild());
            daDefaultTableModel.addRow(new Object[]{info.getName(), ver, info.getModule()});
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
