package eternalhush.manager;

import com.sun.jna.Memory;
import com.sun.jna.Pointer;
import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringEscapeUtils;
import org.apache.commons.lang3.StringUtils;
import eternalhush.console.CommandHandler;
import eternalhush.console.OperationConsole;
import eternalhush.dlcore.CoreConnector;
import eternalhush.dlcore.OperationHandler;
import eternalhush.main.GlobalLogger;
import eternalhush.main.GlobalVariables;

import java.nio.ByteBuffer;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.atomic.AtomicInteger;

public class ExternalModules {
    private OperationConsole currentConsole;
    private String commandName;
    private CommonModule currentModule;

    public ExternalModules(OperationConsole console) {
        currentConsole = console;
    }

    private void logInvalidCommand(String cmd_name, int id) {
        GlobalVariables.GlobalOperCommandLogger.writeIoLog(2, null, null, 12, id, cmd_name, currentConsole.getConsoleId());
    }

    public void processCommand(String[] args, int command_id, int primary_run) {
        commandName = args[0].substring(1);
        GlobalLogger.log(String.format("Dispatching external command '%s'", commandName));
        for (CommonModule module : GlobalVariables.commonModuleList) { //iterate loaded modules

            for (int j = 0; j < module.getCmdCount(); j++) { //iterate commands
                if (commandName.equalsIgnoreCase(module.getCmd(j).CommandName) && (module.getCmd(j).Dependency.equals("default") || module.getCmd(j).Dependency.equals("*"))) {
                    if (module.getCmd(j).getProviderType().equalsIgnoreCase("alias")) {
                        String[] arg_array = ArrayUtils.remove(args, 0);
                        String full_command_line = module.getCmd(j).getProvider();
                        for (String arg : arg_array) {
                            full_command_line = full_command_line.replaceFirst("%%", arg);
                        }
                        full_command_line = full_command_line.replaceAll("%%", "");
                        new CommandHandler().CommonHandler(full_command_line, currentConsole, primary_run);
                        return;
                    }
                    int cnt = 0;
                    if (currentConsole != null) {
                        for (String arg : args) {
                            String[] substr = StringUtils.substringsBetween(arg, "$$", "$$");
                            Map<String, String> envp = currentConsole.getEnv();
                            String temp = arg;
                            if (substr != null) {
                                for (String s : substr) {
                                    temp = temp.replaceFirst("\\$\\$([\\s\\S]+?)\\$\\$", StringEscapeUtils.escapeJava(envp.get(s)).replaceAll("\\\\", "\\\\\\\\"));
                                }
                            }
                            args[cnt] = temp;
                            cnt++;
                        }
                    }

                    ArgumentProcessor processor = new ArgumentProcessor();

                    int real_spec_argc = processor.get_spec_arg_count(args);
                    real_spec_argc -= processor.get_builtin_flags(args).size();

                    String err = processor.check_required(args, module.getCmd(j));
                    if (err != null) {
                        currentConsole.printError("Argument '" + err + "' missing!\n");
                        logInvalidCommand(commandName, command_id);
                        return;
                    }

                    if (real_spec_argc < module.getCmd(j).getRequiredArgCount()
                            || real_spec_argc > module.getCmd(j).getArgCount()) {
                        currentConsole.printError("You provided " + (real_spec_argc) + " arguments, but only " + module.getCmd(j).getRequiredArgCount() + " were expected!\n");
                        logInvalidCommand(commandName, command_id);
                        return;
                    }

                    err = processor.validate(args, module.getCmd(j));
                    if (err != null) {
                        currentConsole.printError("Invalid argument '" + err + "' or value passed!\n");
                        logInvalidCommand(commandName, command_id);
                        return;
                    }
                    currentModule = module;
                    createTask(j, args, command_id, primary_run);
                    return;
                }
            }
        }
        currentConsole.printError("Unrecognized external command \"" + commandName + "\"!\n");
        currentConsole.printError("Type \".help\" to display the available commands.\n");
        logInvalidCommand(commandName, command_id);
    }

    public void processCommand(String[] args, String locked_module, int command_id, int primary_run) {
        commandName = args[0].substring(1);
        GlobalLogger.log(String.format("Dispatching external command '%s' with lock %s", commandName, locked_module));
        for (CommonModule module : GlobalVariables.commonModuleList) { //iterate loaded modules

            for (int j = 0; j < module.getCmdCount(); j++) { //iterate commands
                if (commandName.equalsIgnoreCase(module.getCmd(j).CommandName) && (module.ModuleName.equalsIgnoreCase(locked_module) || module.getCmd(j).Dependency.equals("*") || module.getCmd(j).Dependency.equalsIgnoreCase(locked_module))) {
                    if (module.getCmd(j).getProviderType().equalsIgnoreCase("alias")) {
                        String[] arg_array = ArrayUtils.remove(args, 0);
                        String full_command_line = module.getCmd(j).getProvider();
                        for (String arg : arg_array) {
                            full_command_line = full_command_line.replaceFirst("%%", arg);
                        }
                        full_command_line = full_command_line.replaceAll("%%", "");
                        new CommandHandler().CommonHandler(full_command_line, locked_module, currentConsole, primary_run);
                        return;
                    }
                    int cnt = 0;
                    if (currentConsole != null) {
                        for (String arg : args) {
                            String[] substr = StringUtils.substringsBetween(arg, "$$", "$$");
                            Map<String, String> envp = currentConsole.getEnv();
                            String temp = arg;
                            if (substr != null) {
                                for (String s : substr) {
                                    temp = temp.replaceFirst("\\$\\$([\\s\\S]+?)\\$\\$", StringEscapeUtils.escapeJava(envp.get(s)).replaceAll("\\\\", "\\\\\\\\"));
                                    System.out.println(temp);
                                }
                            }
                            args[cnt] = temp;
                            cnt++;
                        }
                    }

                    ArgumentProcessor processor = new ArgumentProcessor();
                    int real_spec_argc = processor.get_spec_arg_count(args);
                    real_spec_argc -= processor.get_builtin_flags(args).size();
                    // System.out.println(real_spec_argc);
                    String err = processor.check_required(args, module.getCmd(j));
                    if (err != null) {
                        currentConsole.printError("Argument '" + err + "' missing!\n");
                        logInvalidCommand(commandName, command_id);
                        return;
                    }

                    if (real_spec_argc < module.getCmd(j).getRequiredArgCount()
                            || real_spec_argc > module.getCmd(j).getArgCount()) {
                        currentConsole.printError("You provided " + (real_spec_argc) + " arguments, but only " + module.getCmd(j).getRequiredArgCount() + " were expected!\n");
                        logInvalidCommand(commandName, command_id);
                        return;
                    }

                    err = processor.validate(args, module.getCmd(j));
                    if (err != null) {
                        currentConsole.printError("Invalid argument '" + err + "' or value passed!\n");
                        logInvalidCommand(commandName, command_id);
                        return;
                    }

                    currentModule = module;
                    createTask(j, args, command_id, primary_run);
                    return;
                }
            }
        }
        currentConsole.printError("Unrecognized external command \"" + commandName + "\"!\n");
        currentConsole.printError("Type \".help\" to display the available commands.\n");
        logInvalidCommand(args[0], command_id);
    }

    private String[] removeOptArg(String[] array, String argument) {
        return ArrayUtils.remove(array, ArrayUtils.indexOf(array, argument));
    }

    private void createTask(int id, String[] args, int command_id, int primary_run) {
        GlobalLogger.log(String.format("Creating task for core ID: %d CID: %d", id, command_id));
        int flags = 0;
        AtomicInteger task_result = new AtomicInteger();
        Commands currentCommand = currentModule.getCmd(id);

        args = ArrayUtils.remove(args, 0);

        String module =
                currentModule.WorkingDirectory
                        + "\\scripts\\"
                        + currentCommand.getProvider();
        int consoleId = currentConsole.getConsoleId();
        String[] restart_args = {};
        ArgumentProcessor processor = new ArgumentProcessor();
        GlobalLogger.log(String.format("Processing built-in flags"));
        if (args.length > 0) {
            List optional_args = processor.get_builtin_flags(args); //optional arg
            if (optional_args.contains("-silent")) {
                args = removeOptArg(args, "-silent");
                flags |= BuiltInTaskFlags.SILENT;
            }
            if (optional_args.contains("-restart")) {
                args = removeOptArg(args, "-restart");
                flags |= BuiltInTaskFlags.RESTART;
                restart_args = args;
            }
            if (optional_args.contains("-background")) {
                //consoleId = -1;
                flags |= BuiltInTaskFlags.BACKGROUND;
                args = removeOptArg(args, "-background");
                if ((flags & BuiltInTaskFlags.SILENT) == 0)
                    currentConsole.printWarning("[Running script in the background...]\n");
            }
            if (optional_args.contains("-nolog")) {
                args = removeOptArg(args, "-nolog");
                flags |= BuiltInTaskFlags.NOLOG;
                currentConsole.setLogging(false);
                if ((flags & BuiltInTaskFlags.SILENT) == 0)
                    currentConsole.printWarning("[Running script without logging...]\n");
            }
            if (optional_args.contains("-force")) {
                args = removeOptArg(args, "-force");
                flags |= BuiltInTaskFlags.FORCE;
            }

        }
        try {
            GlobalLogger.log(String.format("Transferring execution to core"));
            GlobalVariables.GlobalOperCommandLogger.writeIoLog(1, null, flags, null, command_id, currentCommand.CommandName, currentConsole.getConsoleId());
            if ((flags & BuiltInTaskFlags.BACKGROUND) != 0) {
                String[] finalArgs = args;

                String[] finalRestart_args = restart_args;
                int finalConsoleId = consoleId;

                Thread.UncaughtExceptionHandler h = (th, ex) -> System.out.println("Uncaught exception: " + ex);

                int finalFlags = flags;
                Thread newThread = new Thread(() -> {
                    if ((finalFlags & BuiltInTaskFlags.SILENT) == 0)
                        currentConsole.printWarning("[Started '" + currentCommand.CommandName + "' [" + currentConsole.getHostname() + "]]\n");
                    if ((finalFlags & BuiltInTaskFlags.RESTART) != 0) {
                        OperationHandler op = new OperationHandler();
                        String str_args = "!" + currentCommand.CommandName;
                        for (String s : finalRestart_args) {
                            str_args += " " + s;
                        }

                        op.AddAutostart(currentConsole.getConsoleId(), str_args, (int) (Math.random() * 65536));
                    }
                    ByteBuffer argbuf = processor.pack_args(finalArgs, currentCommand);
                    Pointer ptr = new Memory(argbuf.array().length + 1);
                    for (int i = 0; i < argbuf.array().length; i++) {
                        ptr.setMemory(i, 1, argbuf.array()[i]);
                    }
                    String randomUUID = UUID.randomUUID().toString();
                    currentConsole.setCurrentTaskUID(randomUUID);
                    task_result.set(CoreConnector.Export.RunScript(module, finalConsoleId, finalFlags, argbuf.array().length, ptr, randomUUID));
                    currentConsole.setCurrentTaskUID("");
                    GlobalVariables.GlobalOperCommandLogger.writeIoLog(2, null, finalFlags, task_result.get(), command_id, currentCommand.CommandName, currentConsole.getConsoleId());
                    currentConsole.setLogging(true);
                });
                newThread.setUncaughtExceptionHandler(h);
                newThread.setDaemon(true);
                newThread.start();
            } else {

                if ((flags & BuiltInTaskFlags.RESTART) != 0) {
                    OperationHandler op = new OperationHandler();
                    String str_args = "!" + currentCommand.CommandName;
                    for (String s : restart_args) {
                        str_args += " " + s;
                    }
                    op.AddAutostart(consoleId, str_args, (int) (Math.random() * 65536));
                }
                if ((flags & BuiltInTaskFlags.SILENT) == 0)
                    currentConsole.printWarning("[Started '" + currentCommand.CommandName + "' [" + currentConsole.getHostname() + "]]\n");
                ByteBuffer argbuf = processor.pack_args(args, currentCommand);
                Pointer ptr = new Memory(argbuf.array().length + 1);
                for (int i = 0; i < argbuf.array().length; i++) {
                    ptr.setMemory(i, 1, argbuf.array()[i]);
                }
                String randomUUID = UUID.randomUUID().toString();
                currentConsole.setCurrentTaskUID(randomUUID);
                task_result.set(CoreConnector.Export.RunScript(module, consoleId, flags, argbuf.array().length, ptr, randomUUID));
                currentConsole.setCurrentTaskUID("");
                GlobalVariables.GlobalOperCommandLogger.writeIoLog(2, null, flags, task_result.get(), command_id, currentCommand.CommandName, currentConsole.getConsoleId());
                currentConsole.setLogging(true);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
