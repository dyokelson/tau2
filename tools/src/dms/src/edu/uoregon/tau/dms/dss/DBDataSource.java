/*
 * To do: 1) Add some sanity checks to make sure that multiple metrics really do
 * belong together. For example, wrap the creation of nodes, contexts, threads,
 * global mapping elements, and the like so that they do not occur after the
 * first metric has been loaded. This will not of course ensure 100% that the
 * data is consistent, but it will at least prevent the worst cases.
 */

package edu.uoregon.tau.dms.dss;

import java.util.*;

public class DBDataSource extends DataSource {

    public DBDataSource(Object initializeObject) {
        super();
        this.setMetrics(new Vector());
        this.initializeObject = initializeObject;
    }

    private Object initializeObject;

    public void cancelLoad() {
        return;
    }

    public int getProgress() {
        return 0;
    }

    public void load() {
        try {
            //######
            //Frequently used items.
            //######
            DatabaseAPI databaseAPI = (DatabaseAPI) initializeObject;

            Function function = null;
            UserEvent userEvent = null;
            FunctionProfile functionProfile = null;
            UserEventProfile userEventProfile = null;

            Node node = null;
            Context context = null;
            edu.uoregon.tau.dms.dss.Thread thread = null;
            int nodeID = -1;
            int contextID = -1;
            int threadID = -1;
            int mappingID = -1;

            //Vector localMap = new Vector();
            //######
            //End - Frequently used items.
            //######
            System.out.println("Processing data, please wait ......");
            long time = System.currentTimeMillis();

            int numberOfMetrics = databaseAPI.getNumberOfMetrics();
            System.out.println("Found " + numberOfMetrics + " metrics.");
            for (int i = 0; i < numberOfMetrics; i++) {
                this.addMetric(databaseAPI.getMetricName(i));
                this.getTrialData().increaseVectorStorage();
            }

            //Add the functionProfiles.
            ListIterator l = databaseAPI.getIntervalEvents();
            
            meanData = new Thread(-1,-1,-1, numberOfMetrics);
            meanData.initializeFunctionList(this.getTrialData().getNumFunctions());
            
            while (l.hasNext()) {
                IntervalEvent f = (IntervalEvent) l.next();
                
                function = this.getTrialData().addFunction(f.getName(), numberOfMetrics);

                FunctionProfile meanProfile = new FunctionProfile(function, numberOfMetrics);
                function.setMeanProfile(meanProfile);
                meanData.addFunctionProfile(meanProfile,function.getID());
                
                
                //Add element to the localMap for more efficient lookup later
                // in the function.
                //localMap.add(new FunIndexFunIDPair(f.getIndexID(), id));

                IntervalLocationProfile fdo = f.getMeanSummary();

                if (f.getGroup() != null) {
                    Group group = this.getTrialData().addGroup(f.getGroup());
                    function.addGroup(group);
                    function.setGroupsSet(true);
                    this.setGroupNamesPresent(true);
                }

                for (int i = 0; i < numberOfMetrics; i++) {
                    meanProfile.setExclusive(i, fdo.getExclusive(i));
                    meanProfile.setExclusivePercent(i,
                            fdo.getExclusivePercentage(i));
                    meanProfile.setInclusive(i, fdo.getInclusive(i));
                    meanProfile.setInclusivePercent(i,
                            fdo.getInclusivePercentage(i));
                    meanProfile.setInclusivePerCall(i, fdo.getInclusivePerCall(i));
                    meanProfile.setNumCalls(fdo.getNumCalls());
                    meanProfile.setNumSubr(fdo.getNumSubroutines());

                    if ((this.getTrialData().getMaxMeanExclusiveValue(i)) < fdo.getExclusive(i)) {
                        this.getTrialData().setMaxMeanExclusiveValue(i, fdo.getExclusive(i));
                    }
                    if ((this.getTrialData().getMaxMeanExclusivePercentValue(i)) < fdo.getExclusivePercentage(i)) {
                        this.getTrialData().setMaxMeanExclusivePercentValue(i,
                                fdo.getExclusivePercentage(i));
                    }
                    if ((this.getTrialData().getMaxMeanInclusiveValue(i)) < fdo.getInclusive(i)) {
                        this.getTrialData().setMaxMeanInclusiveValue(i, fdo.getInclusive(i));
                    }
                    if ((this.getTrialData().getMaxMeanInclusivePercentValue(i)) < fdo.getInclusivePercentage(i)) {
                        this.getTrialData().setMaxMeanInclusivePercentValue(i,
                                fdo.getInclusivePercentage(i));
                    }

                    if ((this.getTrialData().getMaxMeanInclusivePerCall(i)) < fdo.getInclusivePerCall(i)) {
                        this.getTrialData().setMaxMeanInclusivePerCall(i,
                                fdo.getInclusivePerCall(i));
                    }

                    if ((this.getTrialData().getMaxMeanNumberOfCalls()) < fdo.getNumCalls()) {
                        this.getTrialData().setMaxMeanNumberOfCalls(fdo.getNumCalls());
                    }

                    if ((this.getTrialData().getMaxMeanNumberOfSubRoutines()) < fdo.getNumSubroutines()) {
                        this.getTrialData().setMaxMeanNumberOfSubRoutines(
                                fdo.getNumSubroutines());
                    }
                }

                
                meanData.setThreadDataAllMetrics();

                function.setMeanValuesSet(true);

                fdo = f.getTotalSummary();
                for (int i = 0; i < numberOfMetrics; i++) {
                    function.setTotalExclusive(i, fdo.getExclusive(i));
                    function.setTotalExclusivePercent(i,
                            fdo.getExclusivePercentage(i));
                    function.setTotalInclusive(i, fdo.getInclusive(i));
                    function.setTotalInclusivePercent(i,
                            fdo.getInclusivePercentage(i));
                    function.setTotalInclusivePerCall(i, fdo.getInclusivePerCall(i));
                    function.setTotalNumCalls(fdo.getNumCalls());
                    function.setTotalNumSubr(fdo.getNumSubroutines());

                }
            }

            //Collections.sort(localMap);

            System.out.println("About to increase storage.");

            l = databaseAPI.getIntervalEventData();
            while (l.hasNext()) {
                IntervalLocationProfile fdo = (IntervalLocationProfile) l.next();
                node = this.getNCT().getNode(fdo.getNode());
                if (node == null)
                    node = this.getNCT().addNode(fdo.getNode());
                context = node.getContext(fdo.getContext());
                if (context == null)
                    context = node.addContext(fdo.getContext());
                thread = context.getThread(fdo.getThread());
                if (thread == null) {
                    thread = context.addThread(fdo.getThread(), numberOfMetrics);
                    thread.setDebug(this.debug());
                    thread.initializeFunctionList(this.getTrialData().getNumFunctions());

                }

                //Get Function and FunctionProfile.

                //Obtain the mapping id from the local map.
                //int pos = Collections.binarySearch(localMap, new
                // FunIndexFunIDPair(fdo.getIntervalEventID(),0));
                //mappingID =
                // ((FunIndexFunIDPair)localMap.elementAt(pos)).paraProfId;

                function = this.getTrialData().getFunction(databaseAPI.getIntervalEvent(fdo.getIntervalEventID()).getName());
                functionProfile = thread.getFunctionProfile(function);
                
                if (functionProfile == null) {
                    functionProfile = new FunctionProfile(function, numberOfMetrics);
                    thread.addFunctionProfile(functionProfile, function.getID());
                }

                for (int i = 0; i < numberOfMetrics; i++) {
                    functionProfile.setExclusive(i, fdo.getExclusive(i));
                    functionProfile.setInclusive(i, fdo.getInclusive(i));
                    functionProfile.setExclusivePercent(i, fdo.getExclusivePercentage(i));
                    functionProfile.setInclusivePercent(i, fdo.getInclusivePercentage(i));
                    functionProfile.setInclusivePerCall(i, fdo.getInclusivePerCall(i));
                    functionProfile.setNumCalls(fdo.getNumCalls());
                    functionProfile.setNumSubr(fdo.getNumSubroutines());

                    //Set the max values.
                    if ((function.getMaxExclusive(i)) < fdo.getExclusive(i))
                        function.setMaxExclusive(i, fdo.getExclusive(i));
                    if ((function.getMaxExclusivePercent(i)) < fdo.getExclusivePercentage(i))
                        function.setMaxExclusivePercent(i,
                                fdo.getExclusivePercentage(i));
                    if ((function.getMaxInclusive(i)) < fdo.getInclusive(i))
                        function.setMaxInclusive(i, fdo.getInclusive(i));
                    if ((function.getMaxInclusivePercent(i)) < fdo.getInclusivePercentage(i))
                        function.setMaxInclusivePercent(i,
                                fdo.getInclusivePercentage(i));
                    if (function.getMaxNumCalls() < fdo.getNumCalls())
                        function.setMaxNumCalls(fdo.getNumCalls());
                    if (function.getMaxNumSubr() < fdo.getNumSubroutines())
                        function.setMaxNumSubr(fdo.getNumSubroutines());
                    if (function.getMaxInclusivePerCall(i) < fdo.getInclusivePerCall(i))
                        function.setMaxInclusivePerCall(i, fdo.getInclusivePerCall(i));

                    if ((thread.getMaxExclusive(i)) < fdo.getExclusive(i))
                        thread.setMaxExclusive(i, fdo.getExclusive(i));
                    if ((thread.getMaxExclusivePercent(i)) < fdo.getExclusivePercentage(i))
                        thread.setMaxExclusivePercent(i, fdo.getExclusivePercentage(i));
                    if ((thread.getMaxInclusive(i)) < fdo.getInclusive(i))
                        thread.setMaxInclusive(i, fdo.getInclusive(i));
                    if ((thread.getMaxInclusivePercent(i)) < fdo.getInclusivePercentage(i))
                        thread.setMaxInclusivePercent(i, fdo.getInclusivePercentage(i));
                    if (thread.getMaxNumCalls() < fdo.getNumCalls())
                        thread.setMaxNumCalls(fdo.getNumCalls());
                    if (thread.getMaxNumSubr() < fdo.getNumSubroutines())
                        thread.setMaxNumSubr(fdo.getNumSubroutines());
                    if (thread.getMaxInclusivePerCall(i) < fdo.getInclusivePerCall(i))
                        thread.setMaxInclusivePerCall(i, fdo.getInclusivePerCall(i));
                }
            }

            l = databaseAPI.getAtomicEvents();
            while (l.hasNext()) {
                AtomicEvent ue = (AtomicEvent) l.next();
                this.getTrialData().addUserEvent(ue.getName());
            }

            l = databaseAPI.getAtomicEventData();
            while (l.hasNext()) {
                AtomicLocationProfile alp = (AtomicLocationProfile) l.next();

                // do we need to do this?
                node = this.getNCT().getNode(alp.getNode());
                if (node == null)
                    node = this.getNCT().addNode(alp.getNode());
                context = node.getContext(alp.getContext());
                if (context == null)
                    context = node.addContext(alp.getContext());
                thread = context.getThread(alp.getThread());
                if (thread == null) {
                    thread = context.addThread(alp.getThread(), numberOfMetrics);
                }


                if (thread.getUsereventList() == null) {
                    thread.initializeUsereventList(this.getTrialData().getNumUserEvents());
                    setUserEventsPresent(true);
                }

                userEvent = this.getTrialData().getUserEvent(databaseAPI.getAtomicEvent(alp.getAtomicEventID()).getName());

                userEventProfile = thread.getUserEvent(userEvent.getID());

                if (userEventProfile == null) {
                    userEventProfile = new UserEventProfile(userEvent);
                    thread.addUserEvent(userEventProfile, userEvent.getID());
                }

                userEventProfile.setUserEventNumberValue(alp.getSampleCount());
                userEventProfile.setUserEventMaxValue(alp.getMaximumValue());
                userEventProfile.setUserEventMinValue(alp.getMinimumValue());
                userEventProfile.setUserEventMeanValue(alp.getMeanValue());
                userEventProfile.setUserEventSumSquared(alp.getSumSquared());
                userEventProfile.updateMax();
                
            }

            System.out.println("Processing callpath data ...");
            if (CallPathUtilFuncs.isAvailable(getTrialData().getFunctions())) {
                setCallPathDataPresent(true);
                if (CallPathUtilFuncs.buildRelations(getTrialData()) != 0) {
                    setCallPathDataPresent(false);
                }
            }

            time = (System.currentTimeMillis()) - time;
            System.out.println("Done processing data file!");
            System.out.println("Time to process file (in milliseconds): " + time);
        } catch (Exception e) {
            e.printStackTrace();
            UtilFncs.systemError(e, null, "SSD01");
        }
    }

    //####################################
    //Instance data.
    //####################################
    private LineData functionDataLine = new LineData();
    private LineData usereventDataLine = new LineData();
    //####################################
    //End - Instance data.
    //####################################
}

/*
 * class FunIndexFunIDPair implements Comparable{ public FunIndexFunIDPair(int
 * functionIndex, int paraProfId){ this.functionIndex = functionIndex;
 * this.paraProfId = paraProfId; }
 * 
 * public int compareTo(Object obj){ return functionIndex -
 * ((FunIndexFunIDPair)obj).functionIndex;}
 * 
 * public int functionIndex; public int paraProfId; }
 */
