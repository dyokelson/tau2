/* -------------------
 * HistogramDemo1.java
 * -------------------
 * (C) Copyright 2004, by Object Refinery Limited.
 *
 */

package client;

import common.*;
import java.io.IOException;
import java.util.List;
import javax.swing.JFrame;
import java.lang.Math;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.statistics.HistogramDataset;
import org.jfree.data.xy.IntervalXYDataset;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RefineryUtilities;
import org.jfree.chart.renderer.xy.XYItemRenderer;
import org.jfree.chart.labels.XYToolTipGenerator;
import org.jfree.data.xy.XYDataset;

/**
 * A demo of the {@link HistogramDataset} class.
 */
public class PerfExplorerHistogramChart extends PerfExplorerChartWindow {

	public PerfExplorerHistogramChart(JFreeChart chart, String name) {
		super(chart, name);
	}

	public static JFrame doHistogram() {
		// for each event, get the variation across all threads.
		PerfExplorerConnection server = PerfExplorerConnection.getConnection();
		// get the data
		RMIChartData data = server.requestChartData(PerfExplorerModel.getModel(), RMIChartData.IQR_DATA);

		// build the chart
		//BoxAndWhiskerCategoryDataset dataset = createDataset(data);
        IntervalXYDataset dataset = createDataset(data);
        JFreeChart chart = createChart(dataset);
        
		ChartPanel panel = new ChartPanel(chart);
		panel.setDisplayToolTips(true);
		XYItemRenderer renderer = chart.getXYPlot().getRenderer();
        renderer.setToolTipGenerator(new XYToolTipGenerator() {
            public String generateToolTip(XYDataset dataset, int arg1, int arg2) {
                return "<html>Event: " + dataset.getSeriesName(arg1) + 
                "<BR>Count: " + dataset.getYValue(arg1, arg2) + "</html>";
            }
        });

		return new PerfExplorerHistogramChart (chart, "Distributions of Significant Events");
	}

    private static IntervalXYDataset createDataset(RMIChartData inData) {
        HistogramDataset dataset = new HistogramDataset();
        List names = inData.getRowLabels();
        double[] values = null;
		for (int i = 0 ; i < inData.getRows(); i++) {
			List doubles = inData.getRowData(i);
			values = new double[doubles.size()];
    		double min = ((double[])(doubles.get(0)))[1];
     		double max = min;  
    		for (int j = 1; j < doubles.size(); j++) {
    			if (min > ((double[])(doubles.get(j)))[1])
    				min = ((double[])(doubles.get(j)))[1];
    			if (max < ((double[])(doubles.get(j)))[1])
    				max = ((double[])(doubles.get(j)))[1];
    		}
    		double range = max - min;
    		//System.out.println("Min: " + min + ", Max: " + max + ", Range: " + range);
    		for (int j = 0; j < doubles.size(); j++) {
    			values[j] = (((double[])(doubles.get(j)))[1]-min)/range;   
    		}
			int bins = 10;
			if (doubles.size() >= 2098)
				bins = 200;
				//bins = Math.max(200, doubles.size() / 100);
			else if (doubles.size() >= 256)
				bins = 50;
			else if (doubles.size() >= 16)
				bins = 20;
         	dataset.addSeries((String)names.get(i), values, bins);
		}
        return dataset;     
    }
    
    /**
     * Creates a sample {@link HistogramDataset}.
     * 
     * @return the dataset.
     */
    private static IntervalXYDataset createDataset() {
        HistogramDataset dataset = new HistogramDataset();
        double[] values = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
        dataset.addSeries("H1", values, 10, 0.0, 10.0);
        return dataset;     
    }

    /**
     * Creates a chart.
     * 
     * @param dataset  a dataset.
     * 
     * @return The chart.
     */
    private static JFreeChart createChart(IntervalXYDataset dataset) {
        JFreeChart chart = ChartFactory.createHistogram(
            "Significant (>2.0% of runtime) Event Histograms", 
            null, 
            null, 
            dataset, 
            PlotOrientation.VERTICAL, 
            true, 
            false, 
            false
        );
        chart.getXYPlot().setForegroundAlpha(0.75f);
        return chart;
    }
        
}
