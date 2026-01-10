package eternalhush.plugin.logviewer;

import com.fasterxml.jackson.databind.SerializationFeature;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlElementWrapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlProperty;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlText;
import eternalhush.console.ConsoleManager;
import eternalhush.main.GlobalVariables;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

public class CmdLogParser {
    XmlMapper mapper;
    protected GlobalCommandLog log;
    String logPath;
    public CmdLogParser(String log_path){
        logPath = log_path;
        mapper = new XmlMapper();
        log = new GlobalCommandLog();
        mapper.enable(SerializationFeature.INDENT_OUTPUT);
        try {
            String xml_content = Files.readString(Paths.get(logPath));
            log = mapper.readValue(xml_content, GlobalCommandLog.class);

        }catch (Exception e){
            System.out.println("LogViewer: Failed to read log file");
            System.out.println(e.getLocalizedMessage());
        }
    }
    public void reparse(){
        try {
            String xml_content = Files.readString(Paths.get(logPath));
            log = mapper.readValue(xml_content, GlobalCommandLog.class);

        }catch (Exception e){
            System.out.println("LogViewer: Failed to re-read log file");
            System.out.println(e.getLocalizedMessage());
        }
    }
    public GlobalCommandLog getLogObject(){
        return log;
    }
    public boolean isTaskRunning(int cid, int taskid){
        ArrayList<LogEntry> id_log_list = this.getConsoleRelatedLogListWithTaskId(cid, taskid);
        for (LogEntry e : id_log_list){
            if (e.entryType.equals(2)){
                return false;
            }
        }
        return true;
    }
    public Set<Integer> getLoggedConsoles(){
        Set<Integer> id_list = new HashSet<>();
        for (LogEntry entry : log.LogEntry){
            id_list.add(entry.console);
        }
        return id_list;
    }
    public ArrayList<LogEntry> getConsoleRelatedLogListWithTaskId(int cid, int taskId){
        ArrayList<LogEntry> log_list = new ArrayList<>();
        for (LogEntry e : log.LogEntry) {
            if (e.console.equals(cid) && e.taskId.equals(taskId)){
                log_list.add(e);
            }
        }
        return log_list;
    }

    public ArrayList<ParsedLogEntry> getConsoleRelatedLogList(int cid){
        ArrayList<ParsedLogEntry> log_list = new ArrayList<>();
        int max_id = Objects.requireNonNull(ConsoleManager.getInstance(cid)).getLastCommandId();
        for (int i = 0; i < max_id; i++){
            ArrayList<LogEntry> id_log_list = this.getConsoleRelatedLogListWithTaskId(cid, i);
            ParsedLogEntry parsedLogEntry = new ParsedLogEntry();
            try {
                for (LogEntry e: id_log_list){
                    if (e.entryType.equals(0)){
                        parsedLogEntry.full_cmd_name = e.cmd_text;
                        parsedLogEntry.timestamp = e.entryTimeStamp;
                        parsedLogEntry.task_id =  e.taskId;
                        parsedLogEntry.console_id = e.console;
                    }
                    if (e.entryType.equals(2)){
                        parsedLogEntry.result = e.result;
                        parsedLogEntry.cmd_name = e.name;
                        parsedLogEntry.flags = e.flags;
                    }
                }
                log_list.add(parsedLogEntry);
            }catch (Exception e){
                e.printStackTrace();
            }
        }
        return log_list;
    }

    static class ParsedLogEntry{
        public Integer result;
        public String timestamp;
        public Integer task_id;
        public Integer console_id;
        public Integer flags;
        public String cmd_name;
        public String full_cmd_name;
    }

    static class GlobalCommandLog{
        @JacksonXmlElementWrapper(useWrapping = false)
        ArrayList<LogEntry> LogEntry = new ArrayList<>();

    }
    static class LogEntry {
        @JacksonXmlProperty(isAttribute = true, localName = "type")
        protected Integer entryType;

        @JacksonXmlProperty(isAttribute = true, localName = "id")
        protected Integer taskId;

        @JacksonXmlProperty(isAttribute = true, localName = "timestamp")
        protected String entryTimeStamp;

        @JacksonXmlProperty(isAttribute = true, localName = "console_id")
        protected Integer console;

        @JacksonXmlProperty(isAttribute = true, localName = "flags")
        protected Integer flags;

        @JacksonXmlProperty(isAttribute = true, localName = "result")
        protected Integer result;

        @JacksonXmlProperty(isAttribute = true, localName = "name")
        protected String name;

        @JacksonXmlText
        protected String cmd_text;
    }
}
