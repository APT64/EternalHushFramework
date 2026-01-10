package eternalhush.plugin.remotefileviewer;

import com.fasterxml.jackson.databind.SerializationFeature;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlElementWrapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlProperty;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlText;
import eternalhush.console.ConsoleManager;

import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

public class FileListingParser {
    XmlMapper mapper;
    protected DirectoryList log;
    String logPath;
    public FileListingParser(String log_path){
        logPath = log_path;
        mapper = new XmlMapper();
        log = new DirectoryList();
        mapper.enable(SerializationFeature.INDENT_OUTPUT);
        try {
            String xml_content = Files.readString(Paths.get(logPath));
            log = mapper.readValue(xml_content, DirectoryList.class);

        }catch (Exception e){
            System.out.println("FileViewer: Failed to read xml file");
            System.out.println(e.getLocalizedMessage());
        }
    }
    public void reparseXml(){
        try {
            String xml_content = Files.readString(Paths.get(logPath));
            log = mapper.readValue(xml_content, DirectoryList.class);
        }catch (Exception e){
            System.out.println("FileViewer: Failed to re-read xml file");
            System.out.println(e.getLocalizedMessage());
        }
    }
    public DirectoryList getResult(){
        return log;
    }

    static class DirectoryList{
        @JacksonXmlProperty(isAttribute = true, localName = "timestamp")
        protected String timestamp;

        @JacksonXmlProperty(isAttribute = true, localName = "taskuid")
        protected String task_uid;

        @JacksonXmlElementWrapper(useWrapping = false)
        ArrayList<FileEntry> FileEntry  = new ArrayList<>();

    }
    static class FileEntry {
        @JacksonXmlProperty(isAttribute = true, localName = "type")
        protected String fileType;

        @JacksonXmlProperty(isAttribute = true, localName = "access")
        protected String fileAccess;

        @JacksonXmlProperty(isAttribute = true, localName = "hidden")
        protected String fileIsHidden;

        @JacksonXmlProperty(isAttribute = true, localName = "system")
        protected String fileIsSystem;

        @JacksonXmlProperty(isAttribute = true, localName = "size")
        protected String fileSize;

        @JacksonXmlProperty(isAttribute = true, localName = "timestamp")
        protected String fileTimestamp;

        @JacksonXmlText
        protected String fileName;
    }
}
