package eternalhush.manager;

import eternalhush.dlcore.ArgumentPacker;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;

public class ArgumentProcessor {
    private ArrayList<SplitCmd> split(String[] args) {
        ArrayList<SplitCmd> c_list = new ArrayList<>();
        for (int i = 0; i < args.length; i++) {
            SplitCmd c = new SplitCmd();
            String s = args[i];
            if (s.startsWith("-")) {
                c.name = s;
                if ((i < args.length - 1 && args[i + 1].startsWith("-")) || i == args.length - 1) {
                    c.value = "$FLAG";
                } else if (i < args.length - 1) {
                    c.value = args[i + 1];
                }
                c_list.add(c);
            }
        }
        return c_list;
    }

    private String searchArgValue(ArrayList<SplitCmd> list, String name) {
        for (SplitCmd c : list) {
            if (c.name.equalsIgnoreCase(name)) {
                return c.value;
            }
        }
        return null;
    }

    private String getArgType(Commands list, String name) {
        for (Arguments a : list.getArgList()) {
            if (a.ArgumentName.equalsIgnoreCase(name)) {
                return a.ArgumentType;
            }
        }
        return null;
    }

    private boolean checkIgnoredList(String name) {
        ArrayList<String> ignoredList = new ArrayList<>();
        ignoredList.add("-force");
        ignoredList.add("-nolog");
        ignoredList.add("-restart");
        ignoredList.add("-background");
        ignoredList.add("-silent");
        return ignoredList.contains(name);
    }

    public ArrayList<String> get_builtin_flags(String[] args) {
        ArrayList<String> flag_list = new ArrayList<>();
        ArrayList<SplitCmd> parsed_args = split(args);

        for (SplitCmd c : parsed_args) {
            if (c.value.equalsIgnoreCase("$FLAG") && checkIgnoredList(c.name)) {
                flag_list.add(c.name);
            }
        }
        return flag_list;
    }

    public ByteBuffer pack_args(String[] args, Commands cmd_def) {
        ArgumentPacker packer = new ArgumentPacker();
        ArrayList<Arguments> arg_list = cmd_def.getArgList();
        ArrayList<SplitCmd> parsed_args = new ArrayList<>();
        parsed_args = split(args);
        for (SplitCmd c : parsed_args) {
            if (getArgType(cmd_def, c.name).equalsIgnoreCase("int")) {
                packer.addArgI(c.name, Integer.parseInt(c.value));
            }
            if (getArgType(cmd_def, c.name).equalsIgnoreCase("string")) {
                packer.addArgS(c.name, c.value);
            }
            if (getArgType(cmd_def, c.name).equalsIgnoreCase("flag")) {
                packer.addArgF(c.name);


            }
        }
        return packer.pack();
    }

    public String check_required(String[] args, Commands cmd_def) {
        ArrayList<Arguments> arg_list = cmd_def.getArgList();
        if (arg_list == null) {
            return null;
        }
        for (Arguments arg_def : arg_list) {
            if (arg_def.IsOptional.equalsIgnoreCase("false") ||
                    arg_def.IsOptional.equalsIgnoreCase("0")) {
                if (!Arrays.asList(args).contains(arg_def.ArgumentName)) {
                    return arg_def.ArgumentName;
                }
            }
        }
        return null;
    }

    public String validate(String[] args, Commands cmd_def) {
        ArrayList<Arguments> arg_list = cmd_def.getArgList();
        ArrayList<SplitCmd> parsed_args = split(args);
        ArrayList<SplitCmd> parsed_args_temp = parsed_args;
        if (arg_list == null) {
            return null;
        }
        for (SplitCmd argname : parsed_args) {
            boolean exist = false;
            if (checkIgnoredList(argname.name)) {
                continue;
            }
            for (Arguments arg_def : arg_list) {
                if (arg_def.ArgumentName.equalsIgnoreCase(argname.name)) {
                    exist = true;
                }
            }
            if (!exist) {
                return argname.name;
            }

        }
        for (Arguments arg_def : arg_list) {
            String value = searchArgValue(parsed_args, arg_def.ArgumentName);
            if (value == null) {
                if ((arg_def.IsOptional.equalsIgnoreCase("true") || arg_def.IsOptional.equalsIgnoreCase("1"))) {
                    continue;
                }
                return arg_def.ArgumentName;
            } else if (value.equalsIgnoreCase("$FLAG")) {
                if (!arg_def.ArgumentType.equalsIgnoreCase("flag")) {
                    return arg_def.ArgumentName;
                }
            } else if (arg_def.ArgumentType.equalsIgnoreCase("int")) {
                try {
                    Integer ivar = Integer.valueOf(value);
                } catch (java.lang.NumberFormatException e) {
                    return arg_def.ArgumentName;
                }
            }
        }
        return null;
    }

    public int get_spec_arg_count(String[] args) {
        ArrayList<SplitCmd> parsed_args = split(args);
        return parsed_args.size();
    }

    class SplitCmd {
        String name;
        String value;
    }
}
