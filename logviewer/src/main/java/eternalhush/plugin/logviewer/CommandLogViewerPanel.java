package eternalhush.plugin.logviewer;

import eternalhush.console.OperationConsole;
import eternalhush.events.ConsoleEventListener;
import eternalhush.events.ConsoleEventSource;
import eternalhush.gui.IconLoader;
import eternalhush.main.GlobalVariables;
import eternalhush.manager.CommonModule;

import javax.swing.*;
import javax.swing.table.DefaultTableModel;

import java.awt.*;

import java.util.ArrayList;


public class CommandLogViewerPanel extends JPanel {
    IconLoader iconLoader;
    ImageIcon statusOk;
    ImageIcon statusBad;
    ImageIcon statusWarn;
    ImageIcon statusRunning;
    CmdLogParser logReader;
    DefaultTableModel daDefaultTableModel;
    OperationConsole referencedCInstance;
    JTable j;
    String[] columnNames = { "", "Id", "Full Command", "Module", "Time"};

    String getModuleName(String cmd) {
        String module_name = null;
        for (CommonModule module : GlobalVariables.commonModuleList){
            for (int i = 0; i < module.getCmdCount(); i++){
                if (module.getCmdName(i).equalsIgnoreCase(cmd)) module_name = module.getModuleName();
            }
        }
        return module_name;
    }
    String fastCmdFromStr(String cmd){
        String local_cmd = cmd;
        local_cmd = local_cmd.replace("\\", "\\\\");
        String split_line[] = local_cmd.split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
        return split_line[0].substring(1);
    }
    void updateRelatedLogTable(OperationConsole console){
        if (referencedCInstance.getConsoleId() == console.getConsoleId()){
            logReader.reparse();
            ArrayList<CmdLogParser.ParsedLogEntry> parsedLogEntries = logReader.getConsoleRelatedLogList(console.getConsoleId());
            if (parsedLogEntries.size() < daDefaultTableModel.getRowCount()) {System.out.println("invalid table "+daDefaultTableModel.getRowCount()); return;}
            daDefaultTableModel.setRowCount(0);
            daDefaultTableModel.setColumnIdentifiers(columnNames);
            setColumnsSize();
            for (CmdLogParser.ParsedLogEntry log_entry : parsedLogEntries) {

                    String module_name = null;
                    if (logReader.isTaskRunning(log_entry.console_id, log_entry.task_id)) {
                        log_entry.result = 1; //task is running (fix)
                        log_entry.cmd_name = fastCmdFromStr(log_entry.full_cmd_name);
                    }
                    if (!log_entry.result.equals(12)) {
                        if (log_entry.full_cmd_name.startsWith(".")) {
                            module_name = "<built-in>";
                        } else {
                            module_name = getModuleName(log_entry.cmd_name);
                            if (module_name == null) module_name = "<INVALID>";
                        }
                        daDefaultTableModel.addRow(new Object[]{getStatusIcon(log_entry.result), log_entry.task_id, log_entry.full_cmd_name, module_name.toLowerCase(), log_entry.timestamp});
                    }

            }
        }
    }
    ImageIcon getStatusIcon(int status){
        if (status == 0) return statusOk;
        else if (status == 12) return statusBad;
            else if(status == 1) return statusRunning;
                else return statusWarn;
    }
    void setColumnsSize(){
        j.getColumnModel().getColumn(0).setMaxWidth(50);
        j.getColumnModel().getColumn(1).setMaxWidth(50);
        j.getColumnModel().getColumn(2).setMaxWidth(1000);
        j.getColumnModel().getColumn(3).setMaxWidth(200);
        j.getColumnModel().getColumn(4).setMaxWidth(250);
    }
    public CommandLogViewerPanel(OperationConsole console){
        super(new BorderLayout());
        iconLoader = new IconLoader();
        referencedCInstance = console;
        statusOk = iconLoader.loadIcon("images/good.png", this.getClass(), 16,16);
        statusBad = iconLoader.loadIcon("images/bad.png",this.getClass(), 16,16);
        statusWarn = iconLoader.loadIcon("images/warn.png",this.getClass(), 16,16);
        statusRunning = iconLoader.loadIcon("images/play.png",this.getClass(), 16,16);

        logReader = new CmdLogParser(GlobalVariables.OperationConf.operCmdLogPath);

        daDefaultTableModel = new DefaultTableModel(0, 0){
            @Override
            public Class<?> getColumnClass(int column) {
                Class<?> returnValue;
                if (column == 0) {
                    returnValue = ImageIcon.class;
                }
                else if(column > 0 && column <= columnNames.length-1){
                    returnValue = String.class;
                }
                else {
                    returnValue = Object.class;
                }
                return returnValue;
            }

        };
        j = new JTable(){
            private static final long serialVersionUID = 1L;

            public boolean isCellEditable(int row, int column) {
                return false;
            }
        };

        // Column Names
        daDefaultTableModel.setColumnIdentifiers(columnNames);

        ConsoleEventSource.addEventListener(new ConsoleEventListener() {
            @Override
            public void onConsoleTextChanged(OperationConsole console) {
            }

            @Override
            public void onStatusUpdated(String text) {}

            @Override
            public void onAllInstancesReady() {
                updateRelatedLogTable(referencedCInstance);
            }

            @Override
            public void onInstanceReady(OperationConsole console) {

            }

            @Override
            public void onCommandFinished(OperationConsole console) {
                updateRelatedLogTable(console);
            }

            @Override
            public void onSessionLocked(OperationConsole operationConsole) {

            }
        });

        j.setModel(daDefaultTableModel);
        j.setRowHeight(20);

        JScrollPane jScrollPane = new JScrollPane(j);
        add(jScrollPane, BorderLayout.CENTER);
    }
}
