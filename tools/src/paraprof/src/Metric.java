/* 
  Metric.java

  Title:      ParaProf
  Author:     Robert Bell
  Description:  
*/

package paraprof;

import javax.swing.tree.*;

public class Metric{
    public Metric(){}
    
    public void setTrial(ParaProfTrial trial){
	this.trial = trial;}

    public ParaProfTrial getTrial(){
	return trial;}
    
    public void setDMTN(DefaultMutableTreeNode defaultMutableTreeNode){
	this.defaultMutableTreeNode = defaultMutableTreeNode;}
    
    public DefaultMutableTreeNode getDMTN(){
	return defaultMutableTreeNode;}
    
    public void setName(String name){
	this.name = name;}
    
    public String getName(){
	return name;}
    
    public void setID(int id){
	this.id = id;}
    
    public int getID(){
	return id;}
    
    public String getIDString(){
	if(trial!=null)
	    return trial.getIDString() + ":" + this.getID() + " - " + this.getName();
	else
	    return ":" + this.getID() + " - " + this.getName();
    }
    
    public String toString(){
	return this.getName();}
    
    ParaProfTrial trial = null;
    DefaultMutableTreeNode defaultMutableTreeNode = null;
    private String name = null;
    private int id = -1;
}
