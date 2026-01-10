package eternalhush.manager;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.dataformat.xml.XmlMapper;
import eternalhush.main.GlobalLogger;
import eternalhush.main.GlobalVariables;
import eternalhush.userinterface.UserInterface;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.util.ArrayList;

public class ModuleLoader {
    CommonModule module;
    MetaInfo meta;
    XmlMapper xmlMapper;
    File moduleDirectory;

    public ModuleLoader() {
        xmlMapper = new XmlMapper();
        moduleDirectory = new File((String) GlobalVariables.globalEnv.get("MODULES_DIRECTORY"));
    }

    private void fixArgumentNames(CommonModule m) {
        for (int i = 0; i < m.getCmdCount(); i++) {
            Commands c = m.getCmd(i);
            if (c.getArgCount() != 0) {
                for (Arguments a : c.getArgList()) {
                    a.ArgumentName = "-" + a.ArgumentName;
                }
            }
        }
    }

    private boolean checkDuplicates(ArrayList<MetaInfo> moduleList) {
        int j = 0;
        int k = 0;
        for (j = 0; j < moduleList.size(); j++) {
            for (k = j + 1; k < moduleList.size(); k++) {
                if (k != j && moduleList.get(k).getModule().equalsIgnoreCase(moduleList.get(j).getModule())) {
                    return true;
                }
            }
        }
        return false;
    }

    public void load(ArrayList<CommonModule> moduleList, ArrayList<MetaInfo> metaList) {
        GlobalLogger.log("Loading modules from " + moduleDirectory.getAbsolutePath());
        for (File files : moduleDirectory.listFiles()) {
            if (files.isDirectory()) {
                File moduleFile = new File(files.getAbsolutePath() + "\\module.xml");
                File metaFile = new File(files.getAbsolutePath() + "\\metainfo.xml");

                try {
                    String moduleContent = Files.readString(moduleFile.toPath());
                    module = xmlMapper.readValue(moduleContent, CommonModule.class);
                    module.setWorkingDirectory(files.getAbsolutePath());

                    new UserInterface().set_global_env(module.ModuleName.toUpperCase() + "_ROOT", module.WorkingDirectory);

                    String metaContent = Files.readString(metaFile.toPath());
                    meta = xmlMapper.readValue(metaContent, MetaInfo.class);
                    GlobalLogger.log(String.format("Processing module %s version %s.%s.%s.%s", meta.ModuleName, meta.major, meta.minor, meta.fix, meta.build));
                    this.fixArgumentNames(module);
                    GlobalLogger.log("Argument names fixed");
                    moduleList.add(module);
                    metaList.add(meta);

                } catch (JsonProcessingException e) {
                    e.printStackTrace();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        if (this.checkDuplicates(metaList)) {
            throw new RuntimeException("Found duplicates in meta files!");
        }
    }

}
