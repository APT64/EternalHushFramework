package manager;
import com.fasterxml.jackson.dataformat.xml.annotation.*;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

@JacksonXmlRootElement(localName = "Module")
public class CommonModule implements Serializable {

    @JacksonXmlProperty(isAttribute = true, localName = "name")
    protected String ModuleName;

    @JacksonXmlElementWrapper(useWrapping = false)
    @JacksonXmlProperty(localName = "Command")
    protected ArrayList<Commands> CommandList;

//  рабочая директория плагина
    protected String WorkingDirectory;

    public int getCmdCount(){
        return CommandList.size();
    }
    public Commands getCmd(int id){
        return CommandList.get(id);
    }
    public String getWorkingDirectory(){
        return WorkingDirectory;
    }
    public void setWorkingDirectory(String directory){
        WorkingDirectory = directory;
        return;
    }
    public String getCmdName(int id){
        return getCmd(id).CommandName;
    }
}

class Commands{

    @JacksonXmlProperty(isAttribute = true, localName = "name")
    protected String CommandName;

    @JacksonXmlProperty(isAttribute = true, localName = "provider")
    protected String ProviderName;

    @JacksonXmlProperty(isAttribute = true, localName = "type")
    protected String ProviderType;


    @JacksonXmlElementWrapper(useWrapping = false)
    @JacksonXmlProperty(localName = "Argument")
    protected ArrayList<Arguments> argument;

    public int getArgCount(){
        if (argument == null){
            return 0;
        }
        return argument.size();
    }
    public Arguments getArg(int id){
        return argument.get(id);
    }
    public ArrayList<Arguments> getArgList(){
        return argument;
    }
    public String getProvider(){
        return ProviderName;
    }
    public String getProviderType(){
        return ProviderType;
    }
}

class Arguments{

    @JacksonXmlProperty(isAttribute = true, localName = "name")
    protected String ArgumentName;

    @JacksonXmlProperty(isAttribute = true, localName = "type")
    protected String ArgumentType;

    @JacksonXmlProperty(localName = "Description")
    protected String ArgumentDescription;
}