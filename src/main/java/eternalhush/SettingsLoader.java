package eternalhush;

import java.io.*;
import java.util.Properties;

public abstract class SettingsLoader {
    private static final String SETTINGS_FILE = "settings.ini";
    private static Properties properties = new Properties();

    static public String getKeyValue(String key){
        try {
            properties.load(new FileInputStream(new File(SETTINGS_FILE)));
        } catch (IOException e) {}
        String value = properties.getProperty(key);
        properties.clear();
        return value;
    }
    static public void setKeyValue(String key, String value){
        try {
            properties.load(new FileInputStream(new File(SETTINGS_FILE)));
            properties.setProperty(key, value);
            properties.store(new FileOutputStream(new File(SETTINGS_FILE)), null);
            properties.clear();
        } catch (IOException e) {}
    }
}
