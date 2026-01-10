package eternalhush.dlcore;

import eternalhush.plugin.BasePluginInterface;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

public class PluginClassLoader {
    public PluginClassLoader() {

    }

    private static ArrayList<String> getClassNamesFromJarFile(File givenFile) throws IOException {
        ArrayList<String> classNames = new ArrayList<>();
        try (JarFile jarFile = new JarFile(givenFile)) {
            Enumeration<JarEntry> e = jarFile.entries();
            while (e.hasMoreElements()) {
                JarEntry jarEntry = e.nextElement();
                if (jarEntry.getName().endsWith(".class")) {
                    String className = jarEntry.getName()
                            .replace("/", ".")
                            .replace(".class", "");
                    classNames.add(className);
                }
            }
            return classNames;
        }
    }

    public BasePluginInterface load(String jar) {
        try {
            File jar_file = new File(jar);
            ArrayList<String> classNamesList = getClassNamesFromJarFile(jar_file);
            URLClassLoader child = new URLClassLoader(
                    new URL[]{jar_file.toURI().toURL()},
                    this.getClass().getClassLoader()
            );
            for (String s : classNamesList) {
                if (!s.contains("$")) {
                    //System.out.println(s);
                    Class classToLoad = Class.forName(s, true, child);
                    if (BasePluginInterface.class.isAssignableFrom(classToLoad)) {
                        return (BasePluginInterface) classToLoad.getDeclaredConstructor().newInstance();
                    }
                    /*Method method = classToLoad.getDeclaredMethod("myMethod");
                    Object instance = classToLoad.newInstance();
                    Object result = method.invoke(instance);
                    System.out.println(result);*/
                }
            }

        } catch (Exception e) {
        }
        return null;
    }
}

