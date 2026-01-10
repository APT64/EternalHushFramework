package eternalhush.main;

public abstract class GlobalLogger {
    public static void log(String text) {
        StackTraceElement ste = Thread.currentThread().getStackTrace()[2];
        System.out.printf("[RUNTIME] (%s:%s:%d): %s\n", ste.getClassName(), ste.getMethodName(), ste.getLineNumber(), text);
    }

    public static void error(String text) {
        StackTraceElement ste = Thread.currentThread().getStackTrace()[2];
        System.out.printf("[ERROR] (%s:%s:%d): %s\n", ste.getClassName(), ste.getMethodName(), ste.getLineNumber(), text);
    }

    public static void warn(String text) {
        StackTraceElement ste = Thread.currentThread().getStackTrace()[2];
        System.out.printf("[WARN] (%s:%s:%d): %s\n", ste.getClassName(), ste.getMethodName(), ste.getLineNumber(), text);
    }
}
