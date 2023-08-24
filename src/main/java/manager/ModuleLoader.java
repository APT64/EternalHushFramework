package manager;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.ArrayList;

public class ModuleLoader {
    CommonModule module;
    XmlMapper xmlMapper;
    File moduleDirectory;

    public ModuleLoader(){
        xmlMapper = new XmlMapper();
        moduleDirectory = new  File(System.getProperty("user.dir") + "\\modules");
    }

    public void load(ArrayList<CommonModule> moduleList){
        for (File files : moduleDirectory.listFiles()) {
            if (files.isDirectory()) {
                File moduleFile = new File(files.getAbsolutePath() + "\\module.xml");

                try {
                    String moduleContent = Files.readString(moduleFile.toPath());
                    module = xmlMapper.readValue(moduleContent, CommonModule.class);
                    module.setWorkingDirectory(files.getAbsolutePath());

                    moduleList.add(module);

                } catch (JsonProcessingException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

}
