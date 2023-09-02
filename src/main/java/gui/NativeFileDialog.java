package gui;

import java.awt.Component;
import java.awt.HeadlessException;
import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.CountDownLatch;
import javafx.application.Platform;
import javafx.embed.swing.JFXPanel;
import javafx.stage.DirectoryChooser;
import javafx.stage.FileChooser;
import javax.swing.JFileChooser;
import static javax.swing.JFileChooser.DIRECTORIES_ONLY;
import javax.swing.filechooser.FileFilter;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.filechooser.FileSystemView;

public class NativeFileDialog extends JFileChooser {

    public static final boolean FX_AVAILABLE;
    private List<File> currentFiles;
    private FileChooser fileChooser;
    private File currentFile;
    private DirectoryChooser directoryChooser;

    static {
        boolean isFx;
        try {
            Class.forName("javafx.stage.FileChooser");
            isFx = true;
            // Initializes JavaFX environment
            JFXPanel jfxPanel = new JFXPanel();
        } catch (ClassNotFoundException e) {
            isFx = false;
        }

        FX_AVAILABLE = isFx;
    }

    public NativeFileDialog() {
        initFxFileChooser(null);
    }

    public NativeFileDialog(String currentDirectoryPath) {
        super(currentDirectoryPath);
        initFxFileChooser(new File(currentDirectoryPath));
    }

    public NativeFileDialog(File currentDirectory) {
        super(currentDirectory);
        initFxFileChooser(currentDirectory);
    }

    public NativeFileDialog(FileSystemView fsv) {
        super(fsv);
        initFxFileChooser(fsv.getDefaultDirectory());
    }

    public NativeFileDialog(File currentDirectory, FileSystemView fsv) {
        super(currentDirectory, fsv);
        initFxFileChooser(currentDirectory);
    }

    public NativeFileDialog(String currentDirectoryPath, FileSystemView fsv) {
        super(currentDirectoryPath, fsv);
        initFxFileChooser(new File(currentDirectoryPath));
    }

    @Override
    public int showOpenDialog(final Component parent) throws HeadlessException {
        if (!FX_AVAILABLE) {
            return super.showOpenDialog(parent);
        }

        final CountDownLatch latch = new CountDownLatch(1);
        Platform.runLater(new Runnable() {
            @Override
            public void run() {

                if (parent != null) {
                    parent.setEnabled(false);
                }

                if (isDirectorySelectionEnabled()) {
                    currentFile = directoryChooser.showDialog(null);
                } else {
                    if (isMultiSelectionEnabled()) {
                        currentFiles = fileChooser.showOpenMultipleDialog(null);
                    } else {
                        currentFile = fileChooser.showOpenDialog(null);
                    }
                }
                latch.countDown();
            }

        });
        try {
            latch.await();
        } catch (InterruptedException ex) {
            throw new RuntimeException(ex);
        } finally {
            if (parent != null) {
                parent.setEnabled(true);
            }
        }

        if (isMultiSelectionEnabled()) {
            if (currentFiles != null) {
                return JFileChooser.APPROVE_OPTION;
            } else {
                return JFileChooser.CANCEL_OPTION;
            }
        } else {
            if (currentFile != null) {
                return JFileChooser.APPROVE_OPTION;
            } else {
                return JFileChooser.CANCEL_OPTION;
            }
        }

    }

    @Override
    public int showSaveDialog(final Component parent) throws HeadlessException {
        if (!FX_AVAILABLE) {
            return super.showSaveDialog(parent);
        }

        final CountDownLatch latch = new CountDownLatch(1);

        Platform.runLater(new Runnable() {
            @Override
            public void run() {

                if (parent != null) {
                    parent.setEnabled(false);
                }

                if (isDirectorySelectionEnabled()) {
                    currentFile = directoryChooser.showDialog(null);
                } else {
                    currentFile = fileChooser.showSaveDialog(null);
                }
                latch.countDown();
            }

        });
        try {
            latch.await();
        } catch (InterruptedException ex) {
            throw new RuntimeException(ex);
        } finally {
            if (parent != null) {
                parent.setEnabled(true);
            }
        }

        if (currentFile != null) {
            return JFileChooser.APPROVE_OPTION;
        } else {
            return JFileChooser.CANCEL_OPTION;
        }
    }

    @Override
    public int showDialog(Component parent, String approveButtonText) {
        if (!FX_AVAILABLE) {
            return super.showDialog(parent, approveButtonText);
        }
        return showOpenDialog(parent);
    }

    @Override
    public File[] getSelectedFiles() {
        if (!FX_AVAILABLE) {
            return super.getSelectedFiles();
        }
        if (currentFiles == null) {
            return null;
        }
        return currentFiles.toArray(new File[currentFiles.size()]);
    }

    @Override
    public File getSelectedFile() {
        if (!FX_AVAILABLE) {
            return super.getSelectedFile();
        }
        return currentFile;
    }

    @Override
    public void setSelectedFiles(File[] selectedFiles) {
        if (!FX_AVAILABLE) {
            super.setSelectedFiles(selectedFiles);
            return;
        }
        if (selectedFiles == null || selectedFiles.length == 0) {
            currentFiles = null;
        } else {
            setSelectedFile(selectedFiles[0]);
            currentFiles = new ArrayList<>(Arrays.asList(selectedFiles));
        }
    }

    @Override
    public void setSelectedFile(File file) {
        if (!FX_AVAILABLE) {
            super.setSelectedFile(file);
            return;
        }
        currentFile = file;
        if (file != null) {
            if (file.isDirectory()) {
                fileChooser.setInitialDirectory(file.getAbsoluteFile());

                if (directoryChooser != null) {
                    directoryChooser.setInitialDirectory(file.getAbsoluteFile());
                }
            } else if (file.isFile()) {
                fileChooser.setInitialDirectory(file.getParentFile());
                fileChooser.setInitialFileName(file.getName());

                if (directoryChooser != null) {
                    directoryChooser.setInitialDirectory(file.getParentFile());
                }
            }

        }
    }

    @Override
    public void setFileSelectionMode(int mode) {
        super.setFileSelectionMode(mode);
        if (!FX_AVAILABLE) {
            return;
        }
        if (mode == DIRECTORIES_ONLY) {
            if (directoryChooser == null) {
                directoryChooser = new DirectoryChooser();
            }
            // Set file again, so directory chooser will be affected by it
            setSelectedFile(currentFile);
            setDialogTitle(getDialogTitle());
        }
    }

    @Override
    public void setDialogTitle(String dialogTitle) {
        if (!FX_AVAILABLE) {
            super.setDialogTitle(dialogTitle);
            return;
        }
        fileChooser.setTitle(dialogTitle);
        if (directoryChooser != null) {
            directoryChooser.setTitle(dialogTitle);
        }
    }

    @Override
    public String getDialogTitle() {
        if (!FX_AVAILABLE) {
            return super.getDialogTitle();
        }
        return fileChooser.getTitle();
    }

    @Override
    public void changeToParentDirectory() {
        if (!FX_AVAILABLE) {
            super.changeToParentDirectory();
            return;
        }
        File parentDir = fileChooser.getInitialDirectory().getParentFile();
        if (parentDir.isDirectory()) {
            fileChooser.setInitialDirectory(parentDir);
            if (directoryChooser != null) {
                directoryChooser.setInitialDirectory(parentDir);
            }
        }
    }

    @Override
    public void addChoosableFileFilter(FileFilter filter) {
        super.addChoosableFileFilter(filter);
        if (!FX_AVAILABLE || filter == null) {
            return;
        }
        if (filter.getClass().equals(FileNameExtensionFilter.class)) {
            FileNameExtensionFilter f = (FileNameExtensionFilter) filter;

            List<String> ext = new ArrayList<>();
            for (String extension : f.getExtensions()) {
                ext.add(extension.replaceAll("^\\*?\\.?(.*)$", "*.$1"));
            }
            fileChooser.getExtensionFilters()
                    .add(new FileChooser.ExtensionFilter(f.getDescription(), ext));
        }
    }

    @Override
    public void setAcceptAllFileFilterUsed(boolean bool) {
        boolean differs = isAcceptAllFileFilterUsed() ^ bool;
        super.setAcceptAllFileFilterUsed(bool);
        if (!FX_AVAILABLE) {
            return;
        }
        if (!differs) {
            return;
        }
        if (bool) {
            fileChooser.getExtensionFilters()
                    .add(new FileChooser.ExtensionFilter("All files", "*.*"));
        } else {
            for (Iterator<FileChooser.ExtensionFilter> it
                 = fileChooser.getExtensionFilters().iterator(); it.hasNext();) {
                FileChooser.ExtensionFilter filter = it.next();
                if (filter.getExtensions().size() == 1
                        && filter.getExtensions().contains("*.*")) {
                    it.remove();
                }
            }
        }

    }

    private void initFxFileChooser(File currentFile) {
        if (FX_AVAILABLE) {
            fileChooser = new FileChooser();
            this.currentFile = currentFile;
            setSelectedFile(currentFile);
        }
    }

}