package edu.uoregon.tau.vis;

import java.awt.*;
import java.awt.image.*;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.*;
import java.text.DecimalFormat;

public class HeatMap extends JPanel implements ImageObserver {

	int[] pixels = null;
	BufferedImage img = null;
	StringBuffer description = null;
	private static final int idealSize = 128;
	private static final ColorScale scale = new ColorScale(ColorScale.ColorSet.RAINBOW);
	public static final String TMPDIR = System.getProperty("user.home") + File.separator + ".ParaProf" + File.separator + "tmp" + File.separator;
	private HeatMapScanner scanner = null; // for tool tips
	private double[][] map = null;
	private DecimalFormat f = new DecimalFormat("0");
	private int size = 128;

	public HeatMap (double[][] map, int size, double max, double min, String description) {
		this.map = map;
		this.size = size;
		this.description = new StringBuffer();
		this.description.append(description);
		double range = max - min;
	
		// build the image data from the cluster results
		pixels = new int[size*size];
		
		// get the size of the image...
		int width = size;
		int height = size;
		
		img = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
		int i = 0;
		for (int x = 0 ; x < width ; x++) {
			for (int y = 0 ; y < height ; y++) {
				if (map[x][y] > 0.0 && range == 0) {
					// this looks inverted, but it is so the sender is on the left, receiver on top
					img.setRGB(y, x, scale.getColor(1f).getRGB());
				} else if (map[x][y] > 0.0) {
					// this looks inverted, but it is so the sender is on the left, receiver on top
					img.setRGB(y, x, scale.getColor((float)((map[x][y]-min)/range)).getRGB());
				}
				i++;
			}
		}
		if (HeatMapWindow.maxCells < size) {
			int factor = (size / HeatMapWindow.maxCells) + 1;
			int newSize = size * factor * HeatMapWindow.viewRatio;
			this.setPreferredSize(new Dimension(newSize,newSize));
			this.setSize(newSize,newSize);
		} else {
			this.setPreferredSize(new Dimension(size,size));
			this.setSize(size,size);
		}
		scanner = new HeatMapScanner(this);
		this.addMouseListener(scanner);
		this.addMouseMotionListener(scanner);
		this.addMouseMotionListener(scanner);
		this.addMouseWheelListener(scanner);
		this.setFocusable(true);  // enables key listener events
		this.addKeyListener(scanner);
	}

	public String getToolTip(Point p) {
		// adjust to zoom
    	int currentSize = this.getPreferredSize().height;
    	double pixelsPerCell = (double)(Math.max(currentSize, HeatMapWindow.viewSize)) / (double)size;
		int x = Math.min((int)((p.getX()) / pixelsPerCell),size-1);  // don't go past the end of the array
		int y = Math.min((int)((p.getY()) / pixelsPerCell),size-1);  // don't go past the end of the array
		// this is inverted - the sender is Y, the receiver is X
		double value = map[y][x];
		String s = "<html>sender = " + y + "<BR>receiver = " + x + "<BR>value = " + f.format(value) + "</html>";
		return s;
	}
		
	public String getImage() {
		String filename = TMPDIR + "clusterImage." + description + ".png";
		File outFile = new File(filename);
		try {
			ImageIO.write(img, "PNG", outFile);
		} catch (IOException e) {
			String error = "ERROR: Couldn't write the virtual topology image!";
			System.err.println(error);
			System.err.println(e.getMessage());
			e.printStackTrace();
		}
		return filename;
	}

	public String getThumbnail() {
		String filename = TMPDIR + "clusterImage.thumb." + description + ".png";
		return filename;
	}

	public void paint(Graphics g) {
		super.paint(g);
		// the size of the component
		Dimension d = getSize();
		// the internal margins of the component
		Insets i = getInsets();
		// draw to fill the entire component
		g.drawImage(img, i.left, i.top, d.width - i.left - i.right, d.height - i.top - i.bottom, this );
		//g.drawImage(img, i.left, i.top, 512, 512, this );
	}

	/**
	 * @return the size
	 */
	public int getMapSize() {
		return size;
	}

	/**
	 * @return the scanner
	 */
	public HeatMapScanner getScanner() {
		return scanner;
	}

}