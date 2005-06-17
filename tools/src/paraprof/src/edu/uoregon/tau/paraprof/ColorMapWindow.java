/*
 * Created on Mar 6, 2005
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
package edu.uoregon.tau.paraprof;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import edu.uoregon.tau.dms.dss.*;

/**
 * @author amorris
 *
 * TODO ...
 */
public class ColorMapWindow extends JFrame implements ActionListener, Observer {

    private DefaultListModel listModel;
    private JList colorList;

    private void addCompItem(Component c, GridBagConstraints gbc, int x, int y, int w, int h) {
        gbc.gridx = x;
        gbc.gridy = y;
        gbc.gridwidth = w;
        gbc.gridheight = h;
        getContentPane().add(c, gbc);
    }

    private void setupMenus() {

        JMenuBar mainMenu = new JMenuBar();

        //File menu.
        JMenu fileMenu = new JMenu("Assign Colors");

        JMenu jMenu = new JMenu("Assign defaults from...");

        Vector trials = ParaProf.paraProfManagerWindow.getLoadedTrials();
        int idx = 0;
        for (Iterator it = trials.iterator(); it.hasNext();) {
            ParaProfTrial ppTrial = (ParaProfTrial) it.next();

            
            
            //            String name = ppTrial.getExperiment().getApplication().getName() + "/"
            //            			  ppTrial.getExperiment().getName() + "/" +
            //            			  ppTrial.getName() + " (" +
            //            			  		"" +
            //            			  		"

            String name = ppTrial.getTrial().getApplicationID() + ":" + ppTrial.getTrial().getExperimentID() + ":" + ppTrial.getTrial().getID()
                    + " - " + ppTrial.getName();

            JMenuItem jMenuItem = new JMenuItem(name);
            jMenuItem.setActionCommand(Integer.toString(idx));
            jMenuItem.addActionListener(this);
            jMenu.add(jMenuItem);
            idx++;
        }

        fileMenu.add(jMenu);

        JMenuItem closeItem = new JMenuItem("Close This Window");
        closeItem.addActionListener(this);
        fileMenu.add(closeItem);

        JMenuItem exitItem = new JMenuItem("Exit ParaProf!");
        exitItem.addActionListener(this);
        fileMenu.add(exitItem);

        mainMenu.add(fileMenu);
        setJMenuBar(mainMenu);

    }

    public ColorMapWindow() {

        setLocation(new Point(100, 100));
        setSize(new Dimension(855, 450));
        setTitle("ParaProf: Color Map");

        setupMenus();

        //Setting up the layout system for the main window.
        Container contentPane = getContentPane();
        contentPane.setLayout(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.insets = new Insets(5, 5, 5, 5);

        gbc.fill = GridBagConstraints.NONE;
        gbc.anchor = GridBagConstraints.CENTER;
        gbc.weightx = 0;
        gbc.weighty = 0;

        //First add the label.
        JLabel titleLabel = new JLabel("Currently Assigned Colors");
        titleLabel.setFont(new Font("SansSerif", Font.PLAIN, 14));
        addCompItem(titleLabel, gbc, 0, 0, 1, 1);

        gbc.fill = GridBagConstraints.BOTH;
        gbc.anchor = GridBagConstraints.WEST;
        gbc.weightx = 0.1;
        gbc.weighty = 0.1;

        //Create and add color list.
        listModel = new DefaultListModel();
        colorList = new JList(listModel);
        colorList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        colorList.setCellRenderer(new ColorMapCellRenderer(ParaProf.colorMap));
        colorList.setSize(500, 300);
        //colorList.addMouseListener(this);
        JScrollPane sp = new JScrollPane(colorList);
        addCompItem(sp, gbc, 0, 1, 1, 2);

        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.NORTH;
        gbc.weightx = 0;
        gbc.weighty = 0;
        JButton deleteButton = new JButton("Remove");
        deleteButton.addActionListener(this);
        addCompItem(deleteButton, gbc, 1, 1, 1, 1);

        gbc.fill = GridBagConstraints.HORIZONTAL;
        gbc.anchor = GridBagConstraints.NORTH;
        gbc.weightx = 0;
        gbc.weighty = 0;
        deleteButton = new JButton("Remove All");
        deleteButton.addActionListener(this);
        addCompItem(deleteButton, gbc, 1, 2, 1, 1);

        //Now populate the color list.

        reload();
    }

    public void reload() {

        listModel.clear();
        for (Iterator it = ParaProf.colorMap.getFunctions(); it.hasNext();) {
            String functionName = (String) it.next();

            listModel.addElement(functionName);

        }
    }

    public void update(Observable o, Object arg) {
        String tmpString = (String) arg;
        if (tmpString.equals("colorMap")) {
            reload();
        }
    }

    public void actionPerformed(ActionEvent evt) {
        try {
            Object EventSrc = evt.getSource();
            String arg = evt.getActionCommand();

            if (EventSrc instanceof JButton) {
                if (arg.equals("Remove")) {
                    int index = colorList.getSelectedIndex();

                    String toRemove = null;
                    for (Iterator it = ParaProf.colorMap.getFunctions(); it.hasNext();) {
                        String functionName = (String) it.next();
                        if (index == 0) {
                            toRemove = functionName;
                        }
                        index--;
                    }
                    ParaProf.colorMap.removeColor(toRemove);

                } else if (arg.equals("Remove All")) {
                    ParaProf.colorMap.removeAll();
                }
            } else if (EventSrc instanceof JMenuItem) {
                if (arg.equals("Close This Window")) {
                    setVisible(false);
                    dispose();
                } else if (arg.equals("Exit ParaProf!")) {
                    setVisible(false);
                    dispose();
                    ParaProf.exitParaProf(0);
                } else {

                    Vector trials = ParaProf.paraProfManagerWindow.getLoadedTrials();

                    int idx = Integer.parseInt(arg);

                    for (Iterator it = trials.iterator(); it.hasNext();) {
                        ParaProfTrial ppTrial = (ParaProfTrial) it.next();

                        if (idx == 0) { // this is the trial that was selected
                            ParaProf.colorMap.assignColorsFromTrial(ppTrial);
                        }
                        idx--;
                    }
                }
            }
        } catch (Exception e) {
            ParaProfUtils.handleException(e);
        }

    }
}

class ColorMapCellRenderer implements ListCellRenderer {
    private ColorMap colorMap;

    ColorMapCellRenderer(ColorMap colorMap) {
        this.colorMap = colorMap;
    }

    public Component getListCellRendererComponent(final JList list, final Object value, final int index,
            final boolean isSelected, final boolean cellHasFocus) {
        return new JPanel() {
            public void paintComponent(Graphics g) {
                super.paintComponent(g);

                String functionName = (String) value;
                Color inColor = colorMap.getColor(functionName);

                int xSize = 0;
                int ySize = 0;
                int maxXNumFontSize = 0;
                int maxXFontSize = 0;
                int maxYFontSize = 0;
                int thisXFontSize = 0;
                int thisYFontSize = 0;
                int barHeight = 0;

                //For this, I will not allow changes in font size.
                barHeight = 12;

                //Create font.
                Font font = new Font(ParaProf.preferencesWindow.getParaProfFont(), Font.PLAIN, barHeight);
                g.setFont(font);
                FontMetrics fmFont = g.getFontMetrics(font);

                maxYFontSize = fmFont.getAscent();
                maxXFontSize = fmFont.stringWidth("0000,0000,0000");

                xSize = getWidth();
                ySize = getHeight();

                String tmpString1 = new String("00" + (ParaProf.colorChooser.getNumberOfColors()));
                maxXNumFontSize = fmFont.stringWidth(tmpString1);

                String tmpString2 = new String(inColor.getRed() + "," + inColor.getGreen() + ","
                        + inColor.getBlue());
                thisXFontSize = fmFont.stringWidth(tmpString2);
                thisYFontSize = maxYFontSize;

                g.setColor(isSelected ? list.getSelectionBackground() : list.getBackground());
                g.fillRect(0, 0, xSize, ySize);

                g.setColor(inColor);
                g.fillRect(5, 1, 50, ySize - 1);

                //Just a sanity check.
                if ((xSize - 50) > 0) {
                    g.setColor(isSelected ? list.getSelectionBackground() : list.getBackground());
                    g.fillRect((5 + maxXNumFontSize + 5 + 50), 0, (xSize - 50), ySize);
                }

                int xStringPos1 = 60;
                int yStringPos1 = (ySize - 5);

                //int xStringPos1 = 5;
                //int yStringPos1 = (ySize - 5);
                g.setColor(isSelected ? list.getSelectionForeground() : list.getForeground());

                int totalNumberOfColors = (ParaProf.colorChooser.getNumberOfColors())
                        + (ParaProf.colorChooser.getNumberOfGroupColors());

                g.drawString(functionName, xStringPos1, yStringPos1);

                //               int xStringPos2 = 50 + (((xSize - 50) - thisXFontSize) / 2);
                //               int yStringPos2 = (ySize - 5);

                //                g.setColor(isSelected ? list.getSelectionForeground() : list.getForeground());
                //                g.drawString(tmpString2, xStringPos2, yStringPos2);
            }

            public Dimension getPreferredSize() {
                int xSize = 0;
                int ySize = 0;
                int maxXNumFontSize = 0;
                int maxXFontSize = 0;
                int maxYFontSize = 0;
                int barHeight = 12;

                //Create font.
                Font font = new Font(ParaProf.preferencesWindow.getParaProfFont(), Font.PLAIN, barHeight);
                Graphics g = getGraphics();
                FontMetrics fmFont = g.getFontMetrics(font);

                String tmpString = new String("00" + (ParaProf.colorChooser.getNumberOfColors()));
                maxXNumFontSize = fmFont.stringWidth(tmpString);

                maxXFontSize = fmFont.stringWidth("0000,0000,0000");
                maxYFontSize = fmFont.getAscent();

                xSize = (maxXNumFontSize + 10 + 50 + maxXFontSize + 20);
                ySize = (10 + maxYFontSize);

                return new Dimension(xSize, ySize);
            }
        };
    }

}
