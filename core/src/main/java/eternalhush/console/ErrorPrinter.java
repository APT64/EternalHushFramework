package eternalhush.console;

import java.awt.*;
import java.io.IOException;
import java.io.OutputStream;

public class ErrorPrinter extends OutputStream {
    private OperationConsole console;

    public ErrorPrinter() {
        console = ConsoleManager.getFirstDefault();
    }

    @Override
    public void write(int b) throws IOException {
        if (console != null) {
            console.print(String.valueOf((char) b), Color.RED);
        }
    }
}
