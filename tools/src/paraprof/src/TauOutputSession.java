/* 
   TauOutputSession.java
   
   Title:      ParaProf
   Author:     Robert Bell
   Description:  
*/

/*
  To do: 
  1) Add some sanity checks to make sure that multiple metrics really do belong together.
  For example, wrap the creation of nodes, contexts, threads, global mapping elements, and
  the like so that they do not occur after the first metric has been loaded.  This will
  not of course ensure 100% that the data is consistent, but it will at least prevent the
  worst cases.
*/

package paraprof;



import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import java.io.*;
import java.util.*;
import dms.dss.*;

public class TauOutputSession extends ParaProfDataSession{

    public TauOutputSession(){
	super();
    }

    /**
     * Initialize the DataSession object.
     *
     * @param	obj	an implementation-specific object required to initialize the DataSession
     */
    public void initialize(Object obj){
	try{
	    //######
	    //Frequently used items.
	    //######
	    int metric = 0;
	    GlobalMappingElement globalMappingElement = null;
	    GlobalThreadDataElement globalThreadDataElement = null;
	    
	    Node node = null;
	    Context context = null;
	    Thread thread = null;
	    int nodeID = -1;
	    int contextID = -1;
	    int threadID = -1;
	    
	    String inputString = null;
	    String s1 = null;
	    String s2 = null;
	    
	    String tokenString;
	    String groupNamesString = null;
	    StringTokenizer genericTokenizer;
	    
	    int mappingID = -1;
	    
	    int numberOfLines = 0;

	    Vector v = null;
	    File[] files = null;
	    //######
	    //End - Frequently used items.
	    //######
	    v = (Vector) obj;
	    for(Enumeration e = v.elements(); e.hasMoreElements() ;){
		System.out.println("Processing data, please wait ......");
		long time = System.currentTimeMillis();
		//Need to call increaseVectorStorage() on all objects that require it.
		this.getGlobalMapping().increaseVectorStorage();
		    
		//Only need to call addDefaultToVectors() if not the first run.
		if(!(this.firstMetric())){
		    if(this.debug())
			System.out.println("Increasing the storage for the new counter.");
		    
		    for(Enumeration e1 = (this.getGlobalMapping().getMapping(0)).elements(); e1.hasMoreElements() ;){
			GlobalMappingElement tmpGME = (GlobalMappingElement) e1.nextElement();
			tmpGME.incrementStorage();
		    }
		    
		    for(Enumeration e2 = (this.getGlobalMapping().getMapping(2)).elements(); e2.hasMoreElements() ;){
			GlobalMappingElement tmpGME = (GlobalMappingElement) e2.nextElement();
			tmpGME.incrementStorage();
		    }
		    
		    for(Enumeration e3 = this.getNCT().getNodes().elements(); e3.hasMoreElements() ;){
			node = (Node) e3.nextElement();
			for(Enumeration e4 = node.getContexts().elements(); e4.hasMoreElements() ;){
			    context = (Context) e4.nextElement();
			    for(Enumeration e5 = context.getThreads().elements(); e5.hasMoreElements() ;){
				thread = (Thread) e5.nextElement();
				thread.incrementStorage();
				for(Enumeration e6 = thread.getFunctionList().elements(); e6.hasMoreElements() ;){
				    GlobalThreadDataElement ref = (GlobalThreadDataElement) e6.nextElement();
				    //Only want to add an element if this mapping existed on this thread.
				    //Check for this.
				    if(ref != null)
					ref.incrementStorage();
				}
			    }
			}
		    }
		    
		    if(this.debug())
			System.out.println("Done increasing the storage for the new counter.");
		}

		Metric metricRef = this.addMetric();
		metric = metricRef.getID();

		files = (File[]) e.nextElement();
		for(int i=0;i<files.length;i++){
		    if(this.debug()){
			System.out.println("######");
			System.out.println("Processing file: " + files[i].getName());
			System.out.println("######");
		    }

		    FileInputStream fileIn = new FileInputStream(files[i]);
		    InputStreamReader inReader = new InputStreamReader(fileIn);
		    BufferedReader br = new BufferedReader(inReader);

		    int[] nct = this.getNCT(files[i].getName());
		    nodeID = nct[0];
		    contextID = nct[1];
		    threadID = nct[2];
		    
		    node = this.getNCT().getNode(nodeID);
		    if(node==null)
			node = this.getNCT().addNode(nodeID);
		    context = node.getContext(contextID);
		    if(context==null)
			context = node.addContext(contextID);
		    thread = context.getThread(threadID);
		    if(thread==null){
			thread = context.addThread(threadID);
			thread.setDebug(this.debug());
		    }
		    if(this.debug())
			System.out.println("n,c,t: " + nct[0] + "," + nct[1] + "," + nct[2]);

		    //####################################
		    //First  Line
		    //####################################
		    inputString = br.readLine();
		    if(inputString == null){
			System.out.println("Error processing file: " + files[i].getName());
			System.out.println("Unexpected end of file!");
			return;
		    }
		    genericTokenizer = new StringTokenizer(inputString, " \t\n\r");
		    //It's first token will be the number of function present.
		    tokenString = genericTokenizer.nextToken();
		    
		    if(i==0){
			//Set the counter name.
			String counterName = getCounterName(inputString);
			//Now set the counter name.
			if(counterName == null)
			    counterName = new String("Time");
			System.out.println("Counter name is: " + counterName);
			
			metricRef.setName(counterName);
		    }
		    //####################################
		    //End - First Line
		    //####################################
		    
		    //####################################
		    //Second Line
		    //####################################
		    inputString = br.readLine();
		    if(inputString == null){
			System.out.println("Error processing file: " + files[i].getName());
			System.out.println("Unexpected end of file!");
			return;
		    }
		    if(i==0){
			//Determine if profile stats or profile calls data is present.
			if(inputString.indexOf("SumExclSqr")!=-1)
			    this.setProfileStatsPresent(true);
		    }
		    //####################################
		    //End - Second Line
		    //####################################

		    //Process the appropriate number of function lines.
		    if(this.debug()){
			System.out.println("######");
			System.out.println("processing functions");
		    }
		    numberOfLines = Integer.parseInt(tokenString);
		    for(int j=0;j<numberOfLines;j++){
			//On the off chance that we start supporting profiles with no functions in them
			//(for example only userevents), don't initialize the function list until now.
			//Just as for userevents, this will cut down on storage.
			if(j==0&&(this.firstMetric()))
			    thread.initializeFunctionList(this.getNumberOfMappings());

			inputString = br.readLine();
			if(inputString==null){
			    System.out.println("Error processing file: " + files[i].getName());
			    System.out.println("Unexpected end of file!");
			    return;
			}
			this.getFunctionDataLine(inputString);
			String groupNames = this.getGroupNames(inputString);
			//Calculate usec/call
			double usecCall = functionDataLine.d0/functionDataLine.i0;
			if(this.debug()){
			    System.out.println("function line: " + inputString);
			    System.out.println("Name:"+functionDataLine.s0);
			    System.out.println("Calls:"+functionDataLine.i0);
			    System.out.println("Subrs:"+functionDataLine.i1);
			    System.out.println("Excl:"+functionDataLine.d0);
			    System.out.println("Incl:"+functionDataLine.d1);
			    System.out.println("SumExclSqr:"+functionDataLine.d2);
			    System.out.println("ProfileCalls:"+functionDataLine.i2);
			    System.out.println("groupNames:"+groupNames);
			}
			if(functionDataLine.i0 !=0){
			    mappingID = this.getGlobalMapping().addGlobalMapping(functionDataLine.s0, 0);
			    globalMappingElement = this.getGlobalMapping().getGlobalMappingElement(mappingID, 0);
			    globalMappingElement.incrementCounter();
			    globalThreadDataElement = thread.getFunction(mappingID);
			    
			    if(globalThreadDataElement == null){
				globalThreadDataElement = new GlobalThreadDataElement(this.getGlobalMapping().getGlobalMappingElement(mappingID, 0), false);
				thread.addFunction(globalThreadDataElement, mappingID);
			    }
			    
			    globalThreadDataElement.setMappingExists();
			    globalThreadDataElement.setExclusiveValue(metric, functionDataLine.d0);
			    globalThreadDataElement.setInclusiveValue(metric, functionDataLine.d1);
			    globalThreadDataElement.setNumberOfCalls(functionDataLine.i0);
			    globalThreadDataElement.setNumberOfSubRoutines(functionDataLine.i1);
			    globalThreadDataElement.setUserSecPerCall(metric, usecCall);
			    
			    globalMappingElement.incrementTotalExclusiveValue(functionDataLine.d0);
			    globalMappingElement.incrementTotalInclusiveValue(functionDataLine.d1);
			    
			    //Set the max values (thread max values are calculated in the Thread class).
			    if((globalMappingElement.getMaxExclusiveValue(metric)) < functionDataLine.d0)
				globalMappingElement.setMaxExclusiveValue(metric, functionDataLine.d0);
			    if((globalMappingElement.getMaxInclusiveValue(metric)) < functionDataLine.d1)
				globalMappingElement.setMaxInclusiveValue(metric, functionDataLine.d1);
			    if(globalMappingElement.getMaxNumberOfCalls() < functionDataLine.i0)
				globalMappingElement.setMaxNumberOfCalls(functionDataLine.i0);
			    if(globalMappingElement.getMaxNumberOfSubRoutines() < functionDataLine.i1)
				globalMappingElement.setMaxNumberOfSubRoutines(functionDataLine.i1);
			    if(globalMappingElement.getMaxUserSecPerCall(metric) < usecCall)
				globalMappingElement.setMaxUserSecPerCall(metric, usecCall);
			    
			    if(!(globalMappingElement.groupsSet())){
				if(this.firstMetric()){ 
				    if(groupNames != null){
					StringTokenizer st = new StringTokenizer(groupNames, " |");
					while (st.hasMoreTokens()){
					    String group = st.nextToken();
					    if(group != null){
						//The potential new group is added here.  If the group is already present, the the addGlobalMapping
						//function will just return the already existing group id.  See the GlobalMapping class for more details.
						int groupID = this.getGlobalMapping().addGlobalMapping(group, 1);
						if((groupID != -1) && (this.debug())){
						    System.out.println("######");
						    System.out.println("Adding " + group + " group with id: " + groupID + " to mapping: " + functionDataLine.s0);
						    System.out.println("######");
						}
						globalMappingElement.addGroup(groupID);
					    }    
					}    
				    }
				}
			    }

			}
			
			if(this.debug()){
			    System.out.println("######");
			    System.out.println("processing profile calls for function: " + functionDataLine.s0);
			}
			//Process the appropriate number of profile call lines.
			for(int k=0;k<functionDataLine.i2;k++){
			    this.setProfileCallsPresent(true);
			    inputString = br.readLine();
			    if(this.debug())
				System.out.println("Profile Calls line: " + inputString);
			    genericTokenizer = new StringTokenizer(inputString, " \t\n\r");
			    //Arguments are evaluated left to right.
			    globalThreadDataElement.addCall(Double.parseDouble(genericTokenizer.nextToken()),
							    Double.parseDouble(genericTokenizer.nextToken()));
			}
			if(this.debug()){
			    System.out.println("######");
			    System.out.println("done processing profile calls for function: " + functionDataLine.s0);
			}
		    }
		    if(this.debug()){
			System.out.println("done processing functions");
			System.out.println("######");
		    }
		    
		    if(this.debug()){
			System.out.println("######");
			System.out.println("processing aggregates");
		    }
		    //Process the appropriate number of aggregate lines.
		    inputString = br.readLine();
		    //A valid profile.*.*.* will always contain this line.
		    if(inputString==null){
			System.out.println("Error processing file: " + files[i].getName());
			System.out.println("Unexpected end of file!");
			return;
		    }
		    genericTokenizer = new StringTokenizer(inputString, " \t\n\r");
		    //It's first token will be the number of aggregates.
		    tokenString = genericTokenizer.nextToken();
		    
		    numberOfLines = Integer.parseInt(tokenString);
		    for(int j=0;j<numberOfLines;j++){
			this.setAggregatesPresent(true);
			inputString = br.readLine();
			if(this.debug())
			    System.out.println("Aggregates line: " + inputString);
		    }
		    if(this.debug()){
			System.out.println("done processing aggregates");
			System.out.println("######");
		    }

		    if(this.firstMetric()){
			//Process the appropriate number of userevent lines.
			if(this.debug()){
			    System.out.println("######");
			    System.out.println("processing userevents");
			}
			inputString = br.readLine();
			if(inputString==null){
			    if(this.debug())
				System.out.println("No userevent data in this file.");
			}
			else{
			    genericTokenizer = new StringTokenizer(inputString, " \t\n\r");
			    //It's first token will be the number of userevents
			    tokenString = genericTokenizer.nextToken();
			    numberOfLines = Integer.parseInt(tokenString);
			    //Skip the heading.
			    br.readLine();
			    for(int j=0;j<numberOfLines;j++){
				if(j==0){
				    thread.initializeUsereventList(this.getNumberOfUserEvents());
				    setUserEventsPresent(true);
				}
				
				inputString = br.readLine();
				if(inputString==null){
				    System.out.println("Error processing file: " + files[i].getName());
				    System.out.println("Unexpected end of file!");
				    return;
				}
				this.getUserEventData(inputString);
				if(this.debug()){
				    System.out.println("userevent line: " + inputString);
				    System.out.println("eventname:"+usereventDataLine.s0);
				    System.out.println("numevents:"+usereventDataLine.i0);
				    System.out.println("max:"+usereventDataLine.d0);
				    System.out.println("min:"+usereventDataLine.d1);
				    System.out.println("mean:"+usereventDataLine.d2);
				    System.out.println("sumsqr:"+usereventDataLine.d3);
				}
				if(usereventDataLine.i0 !=0){
				    mappingID = this.getGlobalMapping().addGlobalMapping(usereventDataLine.s0, 2);
				    globalMappingElement = this.getGlobalMapping().getGlobalMappingElement(mappingID, 2);
				    globalMappingElement.incrementCounter();
				    globalThreadDataElement = thread.getUserevent(mappingID);
				    
				    if(globalThreadDataElement == null){
					globalThreadDataElement = new GlobalThreadDataElement(this.getGlobalMapping().getGlobalMappingElement(mappingID, 2), true);
					thread.addUserevent(globalThreadDataElement, mappingID);
				    }
				    
				    globalThreadDataElement.setUserEventNumberValue(usereventDataLine.i0);
				    globalThreadDataElement.setUserEventMaxValue(usereventDataLine.d0);
				    globalThreadDataElement.setUserEventMinValue(usereventDataLine.d1);
				    globalThreadDataElement.setUserEventMeanValue(usereventDataLine.d2);
				    
				    if((globalMappingElement.getMaxUserEventNumberValue()) < usereventDataLine.i0)
					globalMappingElement.setMaxUserEventNumberValue(usereventDataLine.i0);
				    if((globalMappingElement.getMaxUserEventMaxValue()) < usereventDataLine.d0)
					globalMappingElement.setMaxUserEventMaxValue(usereventDataLine.d0);
				    if((globalMappingElement.getMaxUserEventMinValue()) < usereventDataLine.d1)
					globalMappingElement.setMaxUserEventMinValue(usereventDataLine.d1);
				    if((globalMappingElement.getMaxUserEventMeanValue()) < usereventDataLine.d2)
					globalMappingElement.setMaxUserEventMeanValue(usereventDataLine.d2);
				}
			    }
			}
			if(this.debug()){
			    System.out.println("done processing userevents");
			    System.out.println("######");
			}
		    }
		    thread.setThreadData(metric);
		}
		//Done with this metric, let the global mapping compute the mean values.
		this.getGlobalMapping().computeMeanData(0,metric);
		
		time = (System.currentTimeMillis()) - time;
		System.out.println("Done processing data!");
		System.out.println("Time to process (in milliseconds): " + time);
	    }
	}
        catch(Exception e){
	    ParaProf.systemError(e, null, "SSD01");
	}
    }
    
    //####################################
    //Private Section.
    //####################################

    //######
    //profile.*.*.* string processing methods.
    //######
     private int[] getNCT(String string){
	int[] nct = new int[3];
	StringTokenizer st = new StringTokenizer(string, ".\t\n\r");
	st.nextToken();
	nct[0] = Integer.parseInt(st.nextToken());
	nct[1] = Integer.parseInt(st.nextToken());
	nct[2] = Integer.parseInt(st.nextToken());
	return nct;
    }
  
    private String getCounterName(String inString){
	try{
	    String tmpString = null;
	    int tmpInt = inString.indexOf("_MULTI_");
      
	    if(tmpInt > 0){
		//We are reading data from a multiple counter run.
		//Grab the counter name.
		tmpString = inString.substring(tmpInt+7);
		return tmpString;
	    }
      	    //We are not reading data from a multiple counter run.
	    return tmpString; 
      	}
	catch(Exception e){
	    ParaProf.systemError(e, null, "SSD26");
	}
    
	return null;
    }

    private void getFunctionDataLine(String string){
	try{
	    StringTokenizer st1 = new StringTokenizer(string, "\"");
	    functionDataLine.s0 = st1.nextToken(); //Name
	    
	    StringTokenizer st2 = new StringTokenizer(st1.nextToken(), " \t\n\r");
	    functionDataLine.i0 = Integer.parseInt(st2.nextToken()); //Calls
	    functionDataLine.i1 = Integer.parseInt(st2.nextToken()); //Subroutines
	    functionDataLine.d0 = Double.parseDouble(st2.nextToken()); //Exclusive
	    functionDataLine.d1 = Double.parseDouble(st2.nextToken()); //Inclusive
	    if(this.profileStatsPresent())
		functionDataLine.d2 = Double.parseDouble(st2.nextToken()); //SumExclSqr
	    functionDataLine.i2 = Integer.parseInt(st2.nextToken()); //ProfileCalls
	}
	catch(Exception e){
	    ParaProf.systemError(e, null, "SSD08");
	}
    }

    private String getGroupNames(String string){
	try{  
	    StringTokenizer getMappingNameTokenizer = new StringTokenizer(string, "\"");
 	    getMappingNameTokenizer.nextToken();
	    String str = getMappingNameTokenizer.nextToken();
        
	    //Just do the group check once.
	    if(!(this.groupCheck())){
		//If present, "GROUP=" will be in this token.
		int tmpInt = str.indexOf("GROUP=");
		if(tmpInt > 0){
		    this.setGroupNamesPresent(true);
		}
		this.setGroupCheck(true);
	    }
	    
	    if(groupNamesPresent()){
		 str = getMappingNameTokenizer.nextToken();
		    return str;
	    }
	    //If here, this profile file does not track the group names.
	    return null;
	}
	catch(Exception e){
	    ParaProf.systemError(e, null, "SSD12");
	}
	return null;
    }

    private void getUserEventData(String string){
	try{
	    StringTokenizer st1 = new StringTokenizer(string, "\"");
	    usereventDataLine.s0 = st1.nextToken();

	    StringTokenizer st2 = new StringTokenizer(st1.nextToken(), " \t\n\r");
	    usereventDataLine.i0 = (int) Double.parseDouble(st2.nextToken()); //Number of calls.
	    usereventDataLine.d0 = Double.parseDouble(st2.nextToken()); //Max
	    usereventDataLine.d1 = Double.parseDouble(st2.nextToken()); //Min
	    usereventDataLine.d2 = Double.parseDouble(st2.nextToken()); //Mean
	    usereventDataLine.d3 = Double.parseDouble(st2.nextToken()); //Standard Deviation.
	}
	catch(Exception e){
	    System.out.println("An error occured!");
	    e.printStackTrace();
	}
    }

    //######
    //End - profile.*.*.* string processing methods.
    //######

    //####################################
    //End - Private Section.
    //####################################

    //####################################
    //Instance data.
    //####################################
    private LineData functionDataLine = new LineData();
    private LineData  usereventDataLine = new LineData();
    //####################################
    //End - Instance data.
    //####################################
}
