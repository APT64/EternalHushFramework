package eternalhush.gui;

import eternalhush.dlcore.OperationHandler;

import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.TableModel;
import java.util.ArrayList;

public class CustomTableListener implements TableModelListener {

    OperationHandler handler = new OperationHandler();

    public void tableChanged(TableModelEvent e) {
        int row = e.getFirstRow();
        int column = e.getColumn();
        if (column == 3) {
            TableModel model = (TableModel) e.getSource();
            Boolean checked = (Boolean) model.getValueAt(row, column);
            int id = (int) model.getValueAt(row, 1);
            ArrayList<OperationHandler.TaskInfo> info = handler.QueryAutostart();
            for (int i = 0; i < info.size(); i++) {
                if (info.get(i).task_id == id) {
                    info.get(i).enabled = checked ? 1 : 0;
                    handler.UpdateRestartQueue(info);
                }
            }

        }
    }
}