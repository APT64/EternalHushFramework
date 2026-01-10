package eternalhush.plugin.logviewer;

import eternalhush.console.OperationConsole;

import java.util.ArrayList;

public class InvalidCommandLogViewerPanel extends CommandLogViewerPanel {
    String[] columnNames = { "", "Id", "Full Command", "Time"};
    public InvalidCommandLogViewerPanel(OperationConsole console) {
        super(console);
    }
    @Override
    void updateRelatedLogTable(OperationConsole console){
        if (referencedCInstance.getConsoleId() == console.getConsoleId()){
            logReader.reparse();
            ArrayList<CmdLogParser.ParsedLogEntry> parsedLogEntries = logReader.getConsoleRelatedLogList(console.getConsoleId());
            if (parsedLogEntries.size() < daDefaultTableModel.getRowCount()) {System.out.println("invalid table "+daDefaultTableModel.getRowCount()); return;}
            daDefaultTableModel.setRowCount(0);
            daDefaultTableModel.setColumnIdentifiers(columnNames);
            setColumnsSize();

            for (CmdLogParser.ParsedLogEntry log_entry : parsedLogEntries) {
                if (logReader.isTaskRunning(log_entry.console_id, log_entry.task_id)) {
                    continue;
                }
                if (log_entry.result.equals(12)) {
                    daDefaultTableModel.addRow(new Object[]{statusBad, log_entry.task_id, log_entry.full_cmd_name, log_entry.timestamp});
                }
            }
        }
    }
    @Override
    void setColumnsSize(){
        j.getColumnModel().getColumn(0).setMaxWidth(50);
        j.getColumnModel().getColumn(1).setMaxWidth(50);
        j.getColumnModel().getColumn(2).setMaxWidth(1200);
        j.getColumnModel().getColumn(3).setMaxWidth(250);
    }
}
