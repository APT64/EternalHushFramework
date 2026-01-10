package eternalhush.dlcore;

import eternalhush.console.ConsoleManager;
import eternalhush.main.GlobalVariables;

import java.io.File;
import java.io.FilenameFilter;
import java.sql.*;
import java.util.ArrayList;

public class OperationHandler {
    private Connection connection;
    private PreparedStatement statement;

    public void initConsoleHistory() {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            connection.setAutoCommit(false);
            statement = connection.prepareStatement("CREATE TABLE History(console_id INTEGER, text STRING, time STRING)");
            statement.execute();
            connection.commit();
        } catch (Exception e) {
            e.printStackTrace();
            ConsoleManager.getFirstDefault().printError("Failed to initialize database! (History)" + e.getLocalizedMessage() + "\n");
        } finally {
            try {
                statement.close();
                connection.close();
            } catch (SQLException e) {

            }
        }
    }

    public void initConsoleInstances() {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            connection.setAutoCommit(false);
            statement = connection.prepareStatement("CREATE TABLE Instances(console_id INTEGER, name STRING, logfile STRING, envfile STRING, lockname STRING, last_cid INTEGER)");
            statement.execute();
            connection.commit();
        } catch (Exception e) {
            e.printStackTrace();
            ConsoleManager.getFirstDefault().printError("Failed to initialize database! (Instances)" + e.getLocalizedMessage() + "\n");
        } finally {
            try {
                statement.close();
                connection.close();
            } catch (SQLException e) {
                System.out.println(e.getLocalizedMessage());
            }
        }
    }

    public void initRestartQueue() {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            connection.setAutoCommit(false);
            statement = connection.prepareStatement("CREATE TABLE Tasks(console_id INTEGER, task_id INTEGER, cmd STRING, enabled INTEGER)");
            statement.execute();
            connection.commit();
        } catch (Exception e) {
            System.out.println(e.getLocalizedMessage());
            ConsoleManager.getFirstDefault().printError("Failed to initialize database! (Tasks)" + e.getLocalizedMessage() + "\n");
        } finally {
            try {
                statement.close();
                connection.close();
            } catch (SQLException e) {
                System.out.println(e.getLocalizedMessage());
            }
        }
    }

    public ArrayList<InstanceInfo> getInstancesInfo() {
        ArrayList<InstanceInfo> info = new ArrayList<>();
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("SELECT * FROM Instances");
            ResultSet rs = statement.executeQuery();

            while (rs.next()) {
                InstanceInfo _info = new InstanceInfo();
                _info.console_id = rs.getInt("console_id");
                _info.name = rs.getString("name");
                _info.logpath = rs.getString("logfile");
                _info.envfile = rs.getString("envfile");
                _info.lockname = rs.getString("lockname");
                _info.last_cid = rs.getInt("last_cid");
                info.add(_info);
            }
            rs.close();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
            return info;
        }
    }

    public ArrayList<String> getRestartTask(int id) {
        ArrayList<String> info = new ArrayList<>();
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("SELECT * FROM Tasks");
            ResultSet rs = statement.executeQuery();

            while (rs.next()) {
                int console_id = rs.getInt("console_id");
                String text = rs.getString("cmd");
                int task_id = rs.getInt("task_id");
                int enabled = rs.getInt("enabled");

                if (console_id == id && enabled != 0) {
                    info.add(text);
                }

            }
            rs.close();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
            return info;
        }
    }

    public ArrayList<String> getConsoleHistory(int id) {
        ArrayList<String> history = new ArrayList<>();
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("SELECT * FROM History");
            ResultSet rs = statement.executeQuery();
            while (rs.next()) {
                int console_id = rs.getInt("console_id");
                String text = rs.getString("text");
                String time = rs.getString("time");

                if (console_id == id) {
                    history.add(text);
                }
            }
            rs.close();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return history;
    }

    public void SaveInstance(InstanceInfo info) {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");

            statement = connection.prepareStatement("SELECT * FROM Instances WHERE console_id = ?");
            statement.setInt(1, info.console_id);
            ResultSet rs = statement.executeQuery();

            if (!rs.next()) {
                statement = connection.prepareStatement("INSERT INTO Instances(console_id, name, logfile, envfile, lockname, last_cid) VALUES(?, ?, ?, ?, ?, ?)");
                statement.setInt(1, info.console_id);
                statement.setString(2, info.name);
                statement.setString(3, info.logpath);
                statement.setString(4, info.envfile);
                statement.setString(5, info.lockname);
                statement.setInt(6, info.last_cid);

                statement.executeUpdate();
            } else {
                statement = connection.prepareStatement("UPDATE Instances SET console_id = ?, name = ?, logfile = ?, envfile = ?, lockname = ?, last_cid = ? WHERE console_id = ?");
                statement.setInt(1, info.console_id);
                statement.setString(2, info.name);
                statement.setString(3, info.logpath);
                statement.setString(4, info.envfile);
                statement.setString(5, info.lockname);
                statement.setInt(6, info.last_cid);
                statement.setInt(7, info.console_id);
                statement.executeUpdate();
            }

            rs.close();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }

    public void AddHistory(int id, String cmd, String time_str) {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("INSERT INTO History(console_id, text, time) VALUES(?, ?, ?)");
            statement.setInt(1, id);
            statement.setString(2, cmd);
            statement.setString(3, time_str);

            statement.executeUpdate();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }

    public void AddAutostart(int id, String cmd, int task_id) {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("INSERT INTO Tasks(console_id, task_id, cmd, enabled) VALUES(?, ?, ?, ?)");
            statement.setInt(1, id);
            statement.setInt(2, task_id);
            statement.setString(3, cmd);
            statement.setInt(4, 1);

            statement.executeUpdate();
            statement.close();
            GlobalVariables.CriticalUiComponents.autorunPanel.updateInfo();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }

    public ArrayList<TaskInfo> QueryAutostart() {
        ArrayList<TaskInfo> info = new ArrayList<>();
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("SELECT * FROM Tasks");
            ResultSet rs = statement.executeQuery();

            while (rs.next()) {
                TaskInfo i = new TaskInfo();
                i.cid = rs.getInt("console_id");
                i.cmd = rs.getString("cmd");
                i.task_id = rs.getInt("task_id");
                i.enabled = rs.getInt("enabled");
                info.add(i);
            }
            rs.close();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
            return info;
        }
    }

    public void UpdateRestartQueue(ArrayList<TaskInfo> info) {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            connection.prepareStatement("DELETE FROM Tasks").execute();

            for (TaskInfo i : info) {
                statement = connection.prepareStatement("INSERT INTO Tasks(console_id, task_id, cmd, enabled) VALUES(?, ?, ?, ?)");
                statement.setInt(1, i.cid);
                statement.setInt(2, i.task_id);
                statement.setString(3, i.cmd);
                statement.setInt(4, i.enabled);

                statement.executeUpdate();
            }

            statement.close();

            GlobalVariables.CriticalUiComponents.autorunPanel.updateInfo();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }

    public void RemoveAllCidReferencedObjects(int console_id) {
        File dir = new File(GlobalVariables.OperationConf.operPath);
        File[] files = dir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.toLowerCase().endsWith(".db");
            }
        });
        for (File f : files) {
            try {
                connection = DriverManager.getConnection("jdbc:sqlite:" + f.getAbsolutePath());
                DatabaseMetaData md = connection.getMetaData();
                ResultSet rs = md.getTables(null, null, "%", null);
                rs.next();
                while (rs.next()) {
                    String table_name = rs.getString(3);
                    statement = connection.prepareStatement("DELETE FROM " + table_name + " WHERE console_id = " + console_id);
                    statement.executeUpdate();
                    statement.close();
                }
                connection.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
    public boolean isTableExists(String path, String table){
        try {
            connection = DriverManager.getConnection(path);
            connection.prepareStatement("SELECT name FROM sqlite_master WHERE type='table' AND name='" + table + "'");
            Statement stmt = connection.createStatement();
            ResultSet rs = stmt.executeQuery("SELECT name FROM sqlite_master WHERE type='table' AND name='" + table + "'");
            return rs.next();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return false;
    }
    public void addNewCryptoKey(CryptoKeyInfo key){
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("INSERT INTO KeyStorage(crypto_blob, crypto_blob2, key_name, key_type, algorithm) VALUES(?, ?, ?, ?, ?)");
            statement.setString(1, key.crypto_blob);
            statement.setString(2, key.crypto_blob2);
            statement.setString(3, key.key_name);
            statement.setString(4, key.key_type);
            statement.setString(5, key.algorithm);

            statement.executeUpdate();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
        }
    }
    public ArrayList<CryptoKeyInfo> QueryKeyStorage() {
        ArrayList<CryptoKeyInfo> info = new ArrayList<>();
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            statement = connection.prepareStatement("SELECT * FROM KeyStorage");
            ResultSet rs = statement.executeQuery();

            while (rs.next()) {
                CryptoKeyInfo i = new CryptoKeyInfo();
                i.crypto_blob = rs.getString("crypto_blob");
                i.crypto_blob2 = rs.getString("crypto_blob2");
                i.key_name = rs.getString("key_name");
                i.key_type = rs.getString("key_type");
                i.algorithm = rs.getString("algorithm");
                info.add(i);
            }
            rs.close();
            statement.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                connection.close();
            } catch (SQLException e) {
                e.printStackTrace();
            }
            return info;
        }
    }
    public void initKeyStorage() {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            connection.setAutoCommit(false);
            statement = connection.prepareStatement("CREATE TABLE KeyStorage(crypto_blob STRING, crypto_blob2 STRING, key_name STRING, key_type STRING, algorithm STRING)");
            statement.execute();
            connection.commit();
        } catch (Exception e) {
            e.printStackTrace();
            ConsoleManager.getFirstDefault().printError("Failed to initialize database! (KeyStorage)" + e.getLocalizedMessage() + "\n");
        } finally {
            try {
                statement.close();
                connection.close();
            } catch (SQLException e) {
                System.out.println(e.getLocalizedMessage());
            }
        }
    }
    public void removeKeyFromStorage(String name) {
        try {
            connection = DriverManager.getConnection("jdbc:sqlite:" + GlobalVariables.OperationConf.operPath + "/operation.db");
            connection.setAutoCommit(false);
            statement = connection.prepareStatement(String.format("DELETE FROM KeyStorage WHERE key_name = '%s'", name));
            statement.executeUpdate();
            connection.commit();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            try {
                statement.close();
                connection.close();
            } catch (SQLException e) {
                System.out.println(e.getLocalizedMessage());
            }
        }
    }
    public static class InstanceInfo {
        public int console_id;
        public String name;
        public String logpath;
        public String envfile;
        public String lockname;
        public int last_cid;
    }

    public static class TaskInfo {
        public int cid;
        public String cmd;
        public int task_id;
        public int enabled;
    }

    public static class CryptoKeyInfo {
        public String crypto_blob;
        public String crypto_blob2;
        public String key_name;
        public String algorithm;
        public String key_type;
    }
}