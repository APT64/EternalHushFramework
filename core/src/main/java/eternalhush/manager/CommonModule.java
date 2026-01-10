package eternalhush.manager;

import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlElementWrapper;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlProperty;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlRootElement;

import java.io.Serializable;
import java.util.ArrayList;

@JacksonXmlRootElement(localName = "Module")
public class CommonModule implements Serializable {

    @JacksonXmlProperty(isAttribute = true, localName = "name")
    protected String ModuleName;

    @JacksonXmlElementWrapper(useWrapping = false)
    @JacksonXmlProperty(localName = "Command")
    protected ArrayList<Commands> CommandList;

    protected String WorkingDirectory;

    public int getCmdCount() {
        return CommandList.size();
    }

    public Commands getCmd(int id) {
        return CommandList.get(id);
    }

    public String getWorkingDirectory() {
        return WorkingDirectory;
    }

    public void setWorkingDirectory(String directory) {
        WorkingDirectory = directory;
        return;
    }

    public String getCmdName(int id) {
        return getCmd(id).CommandName;
    }

    public String getCmdProvider(int id) {
        return getCmd(id).ProviderName;
    }

    public String getCmdDependency(int id) {
        return getCmd(id).Dependency;
    }

    public String getCmdProviderType(int id) {
        return getCmd(id).ProviderType;
    }

    public String getCmdDesc(int id) {
        return getCmd(id).CommandDescription;
    }

    public ArrayList<String> getCmdArgumentNameList(int id) {
        ArrayList<String> list = new ArrayList<>();
        Commands c = getCmd(id);
        if (c.getArgCount() > 0) {
            ArrayList<Arguments> a = c.getArgList();
            for (Arguments arg_d : a) {
                list.add(arg_d.ArgumentName);
            }
        }
        return list;
    }

    public ArrayList<String> getCmdArgumentTypeList(int id) {
        ArrayList<String> list = new ArrayList<>();
        Commands c = getCmd(id);
        ArrayList<Arguments> a = c.getArgList();
        for (Arguments arg_d : a) {
            list.add(arg_d.ArgumentType);
        }
        return list;
    }

    public String getModuleName() {
        return ModuleName;
    }
}

class Commands {

    @JacksonXmlProperty(isAttribute = true, localName = "name")
    protected String CommandName;

    @JacksonXmlProperty(isAttribute = true, localName = "provider")
    protected String ProviderName;

    @JacksonXmlProperty(isAttribute = true, localName = "dependency")
    protected String Dependency = "default";

    @JacksonXmlProperty(isAttribute = true, localName = "type")
    protected String ProviderType;

    @JacksonXmlProperty(localName = "Description")
    protected String CommandDescription;

    @JacksonXmlElementWrapper(useWrapping = false)
    @JacksonXmlProperty(localName = "Argument")
    protected ArrayList<Arguments> argument;

    public int getArgCount() {
        if (argument == null) {
            return 0;
        }
        return argument.size();
    }

    public int getRequiredArgCount() {
        if (argument == null) {
            return 0;
        }
        int c = 0;
        for (Arguments arg : argument) {
            if (arg.IsOptional.equalsIgnoreCase("false") ||
                    arg.IsOptional.equalsIgnoreCase("0")) {
                c++;
            }
        }
        return c;
    }

    public Arguments getArg(int id) {
        return argument.get(id);
    }

    public ArrayList<Arguments> getArgList() {
        return argument;
    }

    public String getProvider() {
        return ProviderName;
    }

    public String getProviderType() {
        return ProviderType;
    }
}

class Arguments {

    @JacksonXmlProperty(isAttribute = true, localName = "name")
    protected String ArgumentName;

    @JacksonXmlProperty(isAttribute = true, localName = "type")
    protected String ArgumentType;

    @JacksonXmlProperty(isAttribute = true, localName = "optional")
    protected String IsOptional = "false";

    @JacksonXmlProperty(localName = "Description")
    protected String ArgumentDescription;
}