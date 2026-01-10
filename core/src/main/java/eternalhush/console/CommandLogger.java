package eternalhush.console;

import com.fasterxml.jackson.databind.SerializationFeature;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlElementWrapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlProperty;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlText;
import eternalhush.main.GlobalVariables;

import java.io.File;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;

public class CommandLogger {
    protected GlobalCommandLog log;
    protected OperationConsole operationConsole;
    XmlMapper mapper;

    public CommandLogger() {
        mapper = new XmlMapper();
        log = new GlobalCommandLog();
        mapper.enable(SerializationFeature.INDENT_OUTPUT);
        try {
            if (new File(GlobalVariables.OperationConf.operCmdLogPath).isFile()) {
                String xml_content = Files.readString(Paths.get(GlobalVariables.OperationConf.operCmdLogPath));
                log = mapper.readValue(xml_content, GlobalCommandLog.class);
            }
        } catch (Exception e) {
            System.out.println("Failed to read main operation log");
        }
    }

    public void writeIoLog(Integer type, String text, Integer flags, Integer result, Integer task_id, String cmd_name, int console) {
        String xml;
        LogEntry entry = new LogEntry();
        LocalDateTime now = LocalDateTime.now();
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd;HH:mm:ss");
        String formattedNow = now.format(formatter);
        entry.entryType = type;
        entry.entryTimeStamp = formattedNow;
        entry.cmd_text = text;
        entry.flags = flags;
        entry.result = result;
        entry.taskId = task_id;
        entry.name = cmd_name;
        entry.console = console;

        log.LogEntry.add(entry);

        try {

            xml = mapper.writeValueAsString(log);
            PrintWriter writer = new PrintWriter(new File(GlobalVariables.OperationConf.operCmdLogPath));
            writer.write(xml);
            writer.close();

        } catch (Exception e) {
            log.LogEntry.remove(entry);
        }
    }

    public void clearLogger() {
        log = new GlobalCommandLog();
    }
}

class GlobalCommandLog {
    @JacksonXmlElementWrapper(useWrapping = false)
    ArrayList<LogEntry> LogEntry = new ArrayList<>();

}

class LogEntry {
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