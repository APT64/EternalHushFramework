package eternalhush.gui;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class ImagePanel extends JPanel {

    private BufferedImage image;

    public ImagePanel(String path) {
        try {

            image = ImageIO.read(new File(getClass().getResource("/" + path).toString()));

        } catch (IOException ex) {
            System.out.println(getClass().getResource("/" + path).toString());
        }
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        g.drawImage(image, 0, 0, this); // see javadoc for more info on the parameters
    }

}