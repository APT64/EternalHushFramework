package eternalhush.manager;

import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlProperty;
import com.fasterxml.jackson.dataformat.xml.annotation.JacksonXmlRootElement;

import java.io.Serializable;

@JacksonXmlRootElement(localName = "MetaInfo")
public class MetaInfo implements Serializable {

    @JacksonXmlProperty(localName = "ProductName")
    protected String ProductName;

    @JacksonXmlProperty(localName = "VersionMajor")
    protected String major;

    @JacksonXmlProperty(localName = "VersionMinor")
    protected String minor;

    @JacksonXmlProperty(localName = "VersionFix")
    protected String fix;

    @JacksonXmlProperty(localName = "VersionBuild")
    protected String build;

    @JacksonXmlProperty(localName = "ModuleName")
    protected String ModuleName;

    public int getMinor() {
        return Integer.parseInt(minor);
    }

    public int getMajor() {
        return Integer.parseInt(major);
    }

    public int getFix() {
        return Integer.parseInt(fix);
    }

    public int getBuild() {
        return Integer.parseInt(build);
    }

    public String getName() {
        return ProductName;
    }

    public String getModule() {
        return ModuleName;
    }
}