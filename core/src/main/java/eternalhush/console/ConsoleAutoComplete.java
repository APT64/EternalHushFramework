package eternalhush.console;

import eternalhush.main.GlobalVariables;
import eternalhush.manager.CommonModule;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Scanner;

public class ConsoleAutoComplete {
    private int moduleCount;
    private ArrayList<String> cmdNames;

    public ConsoleAutoComplete(String text, OperationConsole console) throws IOException {
        cmdNames = new ArrayList<>();
        console.setCmdStatus_Running();
        moduleCount = GlobalVariables.commonModuleList.size();
        if (text.equalsIgnoreCase("")) {
            String query = "Do you want to display all " + getCmdCount("") + " commands from " + moduleCount + " modules?\n";
            console.printDefault(query);
            console.setQueryText(query + " (y/N)");
            console.redirectNextLine();
            Scanner scanner = new Scanner(console.redirectInputStream);
            String answer = scanner.nextLine();
            if (answer.length() == 0) {
                answer = "n";
            }
            if (!answer.equalsIgnoreCase("n") && !answer.equalsIgnoreCase("y")) {
                console.printError("Unknown option \"" + answer + "\"");
            } else if (answer.equalsIgnoreCase("y")) {
                printAvailableCommands(console, "");
            }
        } else if (text.startsWith("!") && !text.contains(" ")) {
            if (getCmdCount(text.substring(1)) > 1) {
                printAvailableCommands(console, text.substring(1));
            } else if (getCmdCount(text.substring(1)) == 1) {
                String cmd_name = getFullCmdName(text.substring(1));
                console.setInputText("!" + cmd_name + " ");
            }
        } else if (text.startsWith("!") && text.contains(" ")) {
            String[] split_line = text.substring(1).split("\\s+(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)");
            for (int i = 0; i < split_line.length; i++) {
                String str = split_line[i];
                if (str.charAt(0) == '"' && str.charAt(str.length() - 1) == '"') {
                    split_line[i] = str.substring(1, str.length() - 1);
                }
            }
            ArrayList<String> arglist = null;
            for (CommonModule module : GlobalVariables.commonModuleList) {
                for (int i = 0; i < module.getCmdCount(); i++) {
                    String _cmd_name = module.getCmdName(i);
                    if (split_line[0].equalsIgnoreCase(_cmd_name)) {
                        arglist = module.getCmdArgumentNameList(i);
                    }
                }
            }
            //split_line = ArrayUtils.remove(split_line, 0);
            if (arglist == null) return;
            //ArrayList<String> new_arglist = new ArrayList<String>();
            if (arglist.size() != 0) {
                arglist.removeAll(new HashSet(Arrays.asList(split_line)));
            }
            if (text.endsWith(" ")) {
                console.printDefault("\nAvailable args:\n");
                for (String s : arglist) {
                    console.printDefault(" " + s + " ");
                }
                console.printDefault("\n");
            } else {
                int matched_cnt = 0;
                String last_piece = split_line[split_line.length - 1];
                if (last_piece.startsWith("-")) {
                    for (String arg : arglist) {
                        if (arg.startsWith(last_piece)) {
                            matched_cnt++;
                        }
                    }
                    if (matched_cnt == 1) {
                        for (String arg : arglist) {
                            if (arg.startsWith(last_piece)) {
                                console.setInputText(text + arg.substring(last_piece.length()) + " ");
                            }
                        }
                    }
                    if (matched_cnt > 1) {
                        console.printDefault("\nMatched args:\n");
                        for (String arg : arglist) {
                            if (arg.startsWith(last_piece)) {
                                console.printDefault(" " + arg + " ");
                            }
                        }
                        console.printDefault("\n");
                    }
                }
            }
        }
        console.clrQueryText();
        console.setCmdStatus_Idle();
    }

    private void printAvailableCommands(OperationConsole currentConsole, String mask) {
        currentConsole.printDefault("\nMatched commands:\n");
        for (CommonModule module : GlobalVariables.commonModuleList) {
            for (int i = 0; i < module.getCmdCount(); i++) {
                String cmd_name = module.getCmdName(i);
                if (cmd_name.startsWith(mask)) {
                    if (module.getCmdDependency(i).equalsIgnoreCase("*") || module.getCmdDependency(i).length() == 0) {
                        currentConsole.printDefault(String.format(" %s\n", cmd_name));
                    }
                    if (currentConsole.isSessionLocked() && currentConsole.getLockedModuleName().equalsIgnoreCase(module.getModuleName())) {
                        currentConsole.printDefault(String.format(" %s\n", cmd_name));
                    }
                }
            }
        }
        currentConsole.printDefault("\n");
    }

    private int getCmdCount(String mask) {
        int _cmdCount = 0;
        for (CommonModule module : GlobalVariables.commonModuleList) {
            for (int i = 0; i < module.getCmdCount(); i++) {
                String cmd_name = module.getCmdName(i);
                if (cmd_name.startsWith(mask)) {
                    _cmdCount++;
                }
            }
        }
        return _cmdCount;
    }

    private String getFullCmdName(String mask) {
        for (CommonModule module : GlobalVariables.commonModuleList) {
            for (int i = 0; i < module.getCmdCount(); i++) {
                String cmd_name = module.getCmdName(i);
                if (cmd_name.startsWith(mask)) {
                    return cmd_name;
                }
            }
        }
        return null;
    }
}
