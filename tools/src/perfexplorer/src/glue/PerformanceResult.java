/**
 * 
 */
package glue;

import java.util.Map;
import java.util.Set;

import edu.uoregon.tau.perfdmf.Trial;

/**
 * This interface is defined as the methods all performance results
 * should support.  All operations should be refered to through
 * this interface, whenever possible.
 * 
 * <P>CVS $Id: PerformanceResult.java,v 1.3 2008/04/13 23:51:15 khuck Exp $</P>
 * @author  Kevin Huck
 * @version 2.0
 * @since   2.0
 */
public interface PerformanceResult {

	/**
	 * This method will return the name of the event which has the highest
	 * inclusive time value in the trial.
	 * 
	 * @return the name of the main event
	 */
	public String getMainEvent();

	/**
	 * This method will return a Set of Integers, which represent the
	 * identifiers of the threads of execution in the trial.
	 * 
	 * @return the set of thread identifiers
	 */
	public Set<Integer> getThreads();

	/**
	 * This method will return a Set of Strings, which represent the
	 * names of the events in the trial.
	 * 
	 * @return the set of event names
	 */
	public Set<String> getEvents();

	/**
	 * This method will return a Set of Strings, which represent the 
	 * names of the metrics in the trial.
	 * 
	 * @return the set of metric names
	 */
	public Set<String> getMetrics();

	/**
	 * This method will return a Set of Strings, which represent the
	 * names of the userevents in the trial.
	 * 
	 * @return the set of userevent names
	 */
	public Set<String> getUserEvents();

	/**
	 * This method will return the inclusive value stored in the trial for
	 * the selected thread, event, metric combination.
	 * 
	 * @param thread
	 * @param event
	 * @param metric
	 * @return the inclusive value
	 */
	public double getInclusive(Integer thread, String event, String metric);

	/**
	 * This method will return the exclusive value stored in the trial for
	 * the selected thread, event, metric combination.
	 * 
	 * @param thread
	 * @param event
	 * @param metric
	 * @return the exclusive value
	 */
	public double getExclusive(Integer thread, String event, String metric);

	/**
	 * This method will return the number of times that the specified event
	 * was called on the specified thread of execution.
	 * 
	 * @param thread
	 * @param event
	 * @return the number of calls
	 */
	public double getCalls(Integer thread, String event);

	/**
	 * This method will return the number of subroutines that the specified event
	 * had on the specified thread of execution.
	 * 
	 * @param thread
	 * @param event
	 * @return the number of subroutines
	 */
	public double getSubroutines(Integer thread, String event);
	
	public double getUsereventNumevents(Integer thread, String event);
	public double getUsereventMax(Integer thread, String event);
	public double getUsereventMin(Integer thread, String event);
	public double getUsereventMean(Integer thread, String event);
	public double getUsereventSumsqr(Integer thread, String event);

	/**
	 * This method will save the specified value as the inclusive value for the
	 * specified thread, event, metric combination.
	 * 
	 * @param thread
	 * @param event
	 * @param metric
	 * @param value
	 */
	public void putInclusive(Integer thread, String event, String metric, double value);
	
	/**
	 * This method will save the specified value as the exclusive value for the
	 * specified thread, event, metric combination.
	 * 
	 * @param thread
	 * @param event
	 * @param metric
	 * @param value
	 */
	public void putExclusive(Integer thread, String event, String metric, double value);
	
	/**
	 * This method will save the specified value as the number of calls for the
	 * specified event on the specified thread of execution.
	 * 
	 * @param thread
	 * @param event
	 * @param value
	 */
	public void putCalls(Integer thread, String event, double value);
	
	/**
	 * This method will save the specified value as the number of subroutines for the
	 * specified event on the specified thread of execution.
	 * 
	 * @param thread
	 * @param event
	 * @param value
	 */
	public void putSubroutines(Integer thread, String event, double value);
	
	public void putUsereventNumevents(Integer thread, String event, double value);
	public void putUsereventMax(Integer thread, String event, double value);
	public void putUsereventMin(Integer thread, String event, double value);
	public void putUsereventMean(Integer thread, String event, double value);
	public void putUsereventSumsqr(Integer thread, String event, double value);
	
	/**
	 * This method will return the number of threads in the trial from which this data
	 * was derived.
	 * 
	 * @return the number of threads in the original trial
	 */
	public Integer getOriginalThreads();
	
	/**
	 * This method will return the value stored in the trial for the specified thread,
	 * event, metric, type combination.
	 * 
	 * @param thread
	 * @param event
	 * @param metric
	 * @param type
	 * @return the value
	 * @see #getExclusive
	 * @see #getInclusive
	 * @see #getCalls
	 * @see #getSubroutines
	 * @see AbstractResult#INCLUSIVE
	 * @see AbstractResult#EXCLUSIVE
	 * @see AbstractResult#CALLS
	 * @see AbstractResult#SUBROUTINES
	 * @see AbstractResult#USEREVENT_NUMEVENTS
	 * @see AbstractResult#USEREVENT_MAX
	 * @see AbstractResult#USEREVENT_MIN
	 * @see AbstractResult#USEREVENT_MEAN
	 * @see AbstractResult#USEREVENT_SUMSQR
	 */
	public double getDataPoint(Integer thread, String event, String metric, int type);

	/**
	 * This method will store the specified value in the trial for the specified thread,
	 * event, metric, type combination.
	 * 
	 * @param thread
	 * @param event
	 * @param metric
	 * @param type
	 * @param value
	 * @see #putExclusive
	 * @see #putInclusive
	 * @see #putCalls
	 * @see #putSubroutines
	 * @see AbstractResult#INCLUSIVE
	 * @see AbstractResult#EXCLUSIVE
	 * @see AbstractResult#CALLS
	 * @see AbstractResult#SUBROUTINES
	 * @see AbstractResult#USEREVENT_NUMEVENTS
	 * @see AbstractResult#USEREVENT_MAX
	 * @see AbstractResult#USEREVENT_MIN
	 * @see AbstractResult#USEREVENT_MEAN
	 * @see AbstractResult#USEREVENT_SUMSQR
	 */
	public void putDataPoint(Integer thread, String event, String metric, int type, double value);
	
	/** 
	 * This method will return a string representation of this PerformanceResult.
	 * @return a printable string
	 */
	public String toString();

	/**
	 * This method will return the metric which represents the time metric in the trial.
	 * 
	 * @return the metric name
	 */
	public String getTimeMetric();

	/**
	 * This method will return a Map of values, sorted by the values.  The keys to the map
	 * are the event strings in the trial.
	 * 
	 * @param metric
	 * @param type
	 * @param ascending
	 * @return the Map of values
	 * @see AbstractResult#INCLUSIVE
	 * @see AbstractResult#EXCLUSIVE
	 * @see AbstractResult#CALLS
	 * @see AbstractResult#SUBROUTINES
	 * @see AbstractResult#USEREVENT_NUMEVENTS
	 * @see AbstractResult#USEREVENT_MAX
	 * @see AbstractResult#USEREVENT_MIN
	 * @see AbstractResult#USEREVENT_MEAN
	 * @see AbstractResult#USEREVENT_SUMSQR
	 */
	public Map<String, Double> getSortedByValue(String metric, int type, boolean ascending);

	/**
	 * This method returns the metric name which represents floating point operations.
	 * 
	 * @return the metric name
	 */
	public String getFPMetric();

	/**
	 * This method returns the metric name which represents L1 cache accesses.
	 * 
	 * @return the metric name
	 */
	public String getL1AccessMetric();

	/**
	 * This method returns the metric name which represents L2 cache accesses.
	 * 
	 * @return the metric name
	 */
	public String getL2AccessMetric();

	/**
	 * This method returns the metric name which represents L3 cache accesses.
	 * 
	 * @return the metric name
	 */
	public String getL3AccessMetric();

	/**
	 * This method returns the metric name which represents the L1 cache misses.
	 * 
	 * @return the metric name
	 */
	public String getL1MissMetric();

	/**
	 * This method returns the metric name which represents the L2 cache misses.
	 * 
	 * @return the metric name
	 */
	public String getL2MissMetric();

	/**
	 * This method returns the metric name which represents the L3 cache misses.
	 * 
	 * @return the metric name
	 */
	public String getL3MissMetric();

	/**
	 * This method returns the metric name which represents the total number of instructions.
	 * 
	 * @return the metric name
	 */
	public String getTotalInstructionMetric();
	
	/**
	 * This method returns the Trial to which the performance data is related.
	 * 
	 * @return the Trial
	 */
	public Trial getTrial();
}
