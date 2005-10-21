package client;

import org.jfree.data.xy.AbstractXYDataset;
import org.jfree.data.xy.XYDataset;
import common.RMIChartData;
import clustering.RawDataInterface;
import java.util.List;
import java.text.DecimalFormat;
import java.text.FieldPosition;

/**
 * Dataset to store scatterplot data.
 * The JFreeChart API requires that client applications extend the 
 * AbstractXYDataset class to implement the data to be plotted in a scatterplot.
 * This is essentially a wrapper class around the RawDataInterface class.
 *
 * <P>CVS $Id: CorrelationPlotDataset.java,v 1.2 2005/10/21 20:44:20 khuck Exp $</P>
 * @author  Kevin Huck
 * @version 0.1
 * @since   0.1
 */
public class CorrelationPlotDataset extends AbstractXYDataset implements XYDataset {

	private RMIChartData data = null;
	private List seriesNames = null;
	private int x = 0;
	private int y = 1;
	private boolean useMainValue = false;
	
	/**
	 * Constructor.
	 * 
	 * @param data
	 * @param seriesNames
	 */
	public CorrelationPlotDataset(RMIChartData data) {
		super();
		this.data = data;
		this.seriesNames = data.getRowLabels();
		this.x = x;
		this.y = y;
		this.useMainValue = useMainValue;
	}

	/* (non-Javadoc)
	 * @see org.jfree.data.general.SeriesDataset#getSeriesCount()
	 */
	public int getSeriesCount() {
		// we have n rows, but the first row is the data we are
		// correlating against.
		//return data.getRows() - 1;
		return data.getRows();
		//return 1;
	}

	/* (non-Javadoc)
	 * @see org.jfree.data.general.SeriesDataset#getSeriesName(int)
	 */
	public String getSeriesName(int arg0) {
		//return (String)(seriesNames.get(arg0+1));
		String tmp = (String)(seriesNames.get(arg0));
		return tmp + ", r = " + getCorrelation(0, arg0);
	}

	/* (non-Javadoc)
	 * @see org.jfree.data.xy.XYDataset#getItemCount(int)
	 */
	public int getItemCount(int arg0) {
		//System.out.println("Item " + arg0 + " Count: " + data.getRowData(arg0+1).size());
		//return data.getRowData(arg0+1).size();
		return data.getRowData(arg0).size();
	}

	/* (non-Javadoc)
	 * @see org.jfree.data.xy.XYDataset#getX(int, int)
	 */
	public Number getX(int arg0, int arg1) {
	/*
		// get the n+1 row
		List row = data.getRowData(0);
		// get the mth column from that row
		double[] values = (double[])row.get(arg1);
		System.out.println("x values (" + arg0 + "," + arg1 + "): " + values[0] + ", " + values[1]);
		return new Double(values[y]);
		*/
		// get the n+1 row
		List row = data.getRowData(arg0);
		// get the mth column from that row
		double[] values = (double[])row.get(arg1);
		//System.out.println("x values (" + arg0 + "," + arg1 + "): " + values[0] + ", " + values[1]);
		return new Double(values[x]);
	}

	/* (non-Javadoc)
	 * @see org.jfree.data.xy.XYDataset#getY(int, int)
	 */
	public Number getY(int arg0, int arg1) {
	/*
		// get the data for the main function
		List row = data.getRowData(arg0+1);
		// get the mth column from that row
		double[] values = (double[])row.get(arg1);
		System.out.println("y values (" + arg0 + "," + arg1 + "): " + values[0] + ", " + values[1]);
		return new Double(values[y]);
		*/
		// get the data for the main function
		List row = data.getRowData(arg0);
		// get the mth column from that row
		double[] values = (double[])row.get(arg1);
		//System.out.println("y values (" + arg0 + "," + arg1 + "): " + values[0] + ", " + values[1]);
		return new Double(values[y]);
	}

	public String getCorrelation (int x, int y) {
		double r = 0.0;
		double xAvg = 0.0;
		double yAvg = 0.0;
		double xStDev = 0.0;
		double yStDev = 0.0;
		double sum = 0.0;
		List xRow = data.getRowData(x);
		List yRow = data.getRowData(y);

		for (int i = 0 ; i < xRow.size() ; i++ ) {
			double[] tmp = (double[])xRow.get(i);
			xAvg += tmp[1];
			tmp = (double[])yRow.get(i);
			yAvg += tmp[1];
		}

		// find the average for the first vector
		xAvg = xAvg / xRow.size();
		// find the average for the second vector
		yAvg = yAvg / yRow.size();


		for (int i = 0 ; i < xRow.size() ; i++ ) {
			double[] tmp = (double[])xRow.get(i);
			xStDev += (tmp[1] - xAvg) * (tmp[1] - xAvg);
			tmp = (double[])yRow.get(i);
			yStDev += (tmp[1] - yAvg) * (tmp[1] - yAvg);
		}

		// find the standard deviation for the first vector
		xStDev = xStDev / (xRow.size() - 1);
		xStDev = Math.sqrt(xStDev);
		// find the standard deviation for the second vector
		yStDev = yStDev / (yRow.size() - 1);
		yStDev = Math.sqrt(yStDev);


		// solve for r
		double tmp1 = 0.0;
		double tmp2 = 0.0;
		for (int i = 0 ; i < xRow.size() ; i++ ) {
			double[] tmp = (double[])xRow.get(i);
			tmp1 = (tmp[1] - xAvg) / xStDev;
			tmp = (double[])yRow.get(i);
			tmp2 = (tmp[1] - yAvg) / yStDev;
			r += tmp1 * tmp2;
		}
		r = r / (xRow.size() - 1);

		//System.out.println("Avg(x) = " + xAvg + ", Avg(y) = " + yAvg);
		//System.out.println("Stddev(x) = " + xStDev + ", Stddev(y) = " + yStDev);
		//System.out.println("r = " + r);

		DecimalFormat format = new DecimalFormat("0.00");
		FieldPosition f = new FieldPosition(0);
		StringBuffer s = new StringBuffer();
		format.format(new Double(r), s, f);
		return s.toString();
	}

}
