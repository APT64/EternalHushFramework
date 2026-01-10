package eternalhush.gui;

import eternalhush.dlcore.OperationHandler;
import eternalhush.main.GlobalVariables;

import javax.swing.*;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.util.ArrayList;

public class AutorunPanel extends JPanel {
    JTable autoruntable;
    OperationHandler handler = new OperationHandler();

    AutorunPanel() {
        super(new BorderLayout());
        autoruntable = new JTable() {

            private static final long serialVersionUID = 1L;

            @Override
            public Class getColumnClass(int column) {
                switch (column) {
                    case 0:
                        return Integer.class;
                    case 1:
                        return Integer.class;
                    case 2:
                        return String.class;
                    default:
                        return Boolean.class;
                }
            }
        };

        DefaultTableCellRenderer centerRenderer = new DefaultTableCellRenderer();
        centerRenderer.setHorizontalAlignment(JLabel.CENTER);
        autoruntable.setDefaultRenderer(String.class, centerRenderer);
        autoruntable.setDefaultRenderer(Integer.class, centerRenderer);

        updateInfo();

        JScrollPane scrollPane = new JScrollPane(autoruntable);

        add(scrollPane, BorderLayout.CENTER);

        GlobalVariables.CriticalUiComponents.autorunPanel = this;
    }

    public void updateInfo() {
        DefaultTableModel dm = new DefaultTableModel() {
            @Override
            public boolean isCellEditable(int row, int col) {
                return col == 3;
            }
        };
        dm.addTableModelListener(new CustomTableListener());
        String[] rows = {"Console Id", "Task Id", "Command", "Enabled"};

        dm.setColumnIdentifiers(rows);

        ArrayList<OperationHandler.TaskInfo> info = handler.QueryAutostart();
        for (OperationHandler.TaskInfo t : info) {
            dm.addRow(new Object[]{t.cid, t.task_id, t.cmd, t.enabled > 0});
        }
        autoruntable.setModel(dm);
    }
}
