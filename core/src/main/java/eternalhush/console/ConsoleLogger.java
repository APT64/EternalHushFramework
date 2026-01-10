package eternalhush.console;

import com.fasterxml.jackson.databind.SerializationFeature;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlElementWrapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlProperty;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlText;

import java.io.File;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;

public class ConsoleLogger {
    protected TextLog log;
    protected OperationConsole operationConsole;
    XmlMapper mapper;

    public ConsoleLogger(OperationConsole console) {
        mapper = new XmlMapper();
        log = new TextLog();
        operationConsole = console;
        mapper.enable(SerializationFeature.INDENT_OUTPUT);
    }

    public void readPreviousLog() {
        try {
            System.out.println(operationConsole.getLogPath());
            if (new File(operationConsole.getLogPath()).isFile()) {
                String xml_content = Files.readString(Paths.get(operationConsole.getLogPath()));
                log = mapper.readValue(xml_content, TextLog.class);
            }
        } catch (Exception e) {
            System.out.println("Failed to read previous console log");
        }
    }

    public void writeIoLog(int type, String text) {
        String xml = null;
        TextEntry entry = new TextEntry();
        entry.status = type;
        entry.text = text;
        log.TextEntry.add(entry);

        try {

            xml = mapper.writeValueAsString(log);
            PrintWriter writer = new PrintWriter(new File(operationConsole.getLogPath()));
            writer.write(xml);
            writer.close();

        } catch (Exception e) {
            System.out.println(entry.text);
            log.TextEntry.remove(entry);
            operationConsole.setLogging(false);
            operationConsole.printWarning("[Logger was crashed! Some information is not saved]\n");
            operationConsole.setLogging(true);
        }
    }

    public void clearLogger() {
        log = new TextLog();
    }
}

class TextLog {
    @JacksonXmlElementWrapper(useWrapping = false)
    ArrayList<TextEntry> TextEntry = new ArrayList<>();

}

class TextEntry {
    @JacksonXmlProperty(isAttribute = true, localName = "status")
    protected int status;

    @JacksonXmlText
    protected String text;
}