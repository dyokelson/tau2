/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.acl.lanl.gov/tau		           **
*****************************************************************************
**    Copyright 1997  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/***************************************************************************
**	File 		: FunctionInfo.h				  **
**	Description 	: TAU Profiling Package				  **
**	Author		: Sameer Shende					  **
**	Contact		: sameer@cs.uoregon.edu sameer@acl.lanl.gov 	  **
**	Flags		: Compile with				          **
**			  -DPROFILING_ON to enable profiling (ESSENTIAL)  **
**			  -DPROFILE_STATS for Std. Deviation of Excl Time **
**			  -DSGI_HW_COUNTERS for using SGI counters 	  **
**			  -DPROFILE_CALLS  for trace of each invocation   **
**			  -DSGI_TIMERS  for SGI fast nanosecs timer	  **
**			  -DTULIP_TIMERS for non-sgi Platform	 	  **
**			  -DPOOMA_STDSTL for using STD STL in POOMA src   **
**			  -DPOOMA_TFLOP for Intel Teraflop at SNL/NM 	  **
**			  -DPOOMA_KAI for KCC compiler 			  **
**			  -DDEBUG_PROF  for internal debugging messages   **
**                        -DPROFILE_CALLSTACK to enable callstack traces  **
**	Documentation	: See http://www.acl.lanl.gov/tau	          **
***************************************************************************/

#ifndef _FUNCTIONINFO_H_
#define _FUNCTIONINFO_H_

/////////////////////////////////////////////////////////////////////
//
// class FunctionInfo
//
// This class is intended to be instantiated once per function
// (or other code block to be timed) as a static variable.
//
// It will be constructed the first time the function is called,
// and that constructor registers this object (and therefore the
// function) with the timer system.
//
//////////////////////////////////////////////////////////////////////


//#define STORAGE(type, variable) map<int, type> variable 
//#define STORAGE(type, variable) vector<type > variable 
#define STORAGE(type, variable) type  variable[TAU_MAX_THREADS] 
#define NOTHREADSTORAGE(type, variable) type variable[TAU_MAX_THREADS] 


class FunctionInfo
{
public:
	// Construct with the name of the function and its type.
	FunctionInfo(const char* name, const char * type, 
          TauGroup_t ProfileGroup = TAU_DEFAULT, 
	  const char *ProfileGroupName = "TAU_DEFAULT", bool InitData = false,
	  int tid = RtsLayer::myThread());
	FunctionInfo(const char* name, string& type, 
	  TauGroup_t ProfileGroup = TAU_DEFAULT,
	  const char *ProfileGroupName = "TAU_DEFAULT", bool InitData = false,
	  int tid = RtsLayer::myThread());
	FunctionInfo(string& name, string& type, 
	  TauGroup_t ProfileGroup = TAU_DEFAULT,
	  const char *ProfileGroupName = "TAU_DEFAULT", bool InitData = false,
	  int tid = RtsLayer::myThread());
	FunctionInfo(string& name, const char * type, 
	  TauGroup_t ProfileGroup = TAU_DEFAULT,
	  const char *ProfileGroupName = "TAU_DEFAULT", bool InitData = false,
	  int tid = RtsLayer::myThread());

	FunctionInfo(const FunctionInfo& X) ;
	// When we exit, we have to clean up.
	~FunctionInfo();
        FunctionInfo& operator= (const FunctionInfo& X) ;

	void FunctionInfoInit(TauGroup_t PGroup, const char *PGroupName, 
	  bool InitData, int tid );
        

	// Tell it about a function call finishing.
	inline void ExcludeTime(double t, int tid);
	// Removing void IncludeTime(double t, int tid);
 	// and replacing with 
        inline void AddInclTime(double t, int tid);
	inline void AddExclTime(double t, int tid);
	inline void IncrNumCalls(int tid);
        inline void IncrNumSubrs(int tid);
	inline bool GetAlreadyOnStack(int tid);
	inline void SetAlreadyOnStack(bool value, int tid);  

	// A container of all of these.
	// The ctor registers with this.
        
	//static vector<FunctionInfo*> FunctionDB[TAU_MAX_THREADS];

#ifdef PROFILE_CALLS
	list < pair<double,double> > *ExclInclCallList; 
	// Make this a ptr to a list so that ~FunctionInfo doesn't destroy it.
	// time spent in each call

	int AppendExclInclTimeThisCall(double ex, double in); 
	// to ExclInclCallList
#endif // PROFILE_CALLS




#ifdef PROFILE_CALLSTACK 
  	double InclTime_cs;
  	double ExclTime_cs;
#endif  // PROFILE_CALLSTACK

private:

	// A record of the information unique to this function.
	// Statistics about calling this function.
#if (defined(PTHREADS) || defined(TULIPTHREADS))
	STORAGE(long, NumCalls);
	STORAGE(long, NumSubrs);
	STORAGE(double, ExclTime);
	STORAGE(double, InclTime);
	STORAGE(bool, AlreadyOnStack);
#ifdef PROFILE_STATS
	STORAGE(double, SumExclSqr);
#endif //PROFILE_STATS 
#else 
	NOTHREADSTORAGE(long, NumCalls);
	NOTHREADSTORAGE(long, NumSubrs);
	NOTHREADSTORAGE(double, ExclTime);
	NOTHREADSTORAGE(double, InclTime);
	NOTHREADSTORAGE(bool, AlreadyOnStack);
#ifdef PROFILE_STATS
	NOTHREADSTORAGE(double, SumExclSqr);
#endif //PROFILE_STATS 
#endif // PTHREADS
	// Expands macro as map<int, long> NumCalls; etc. 

public:
	string Name;
	string Type;
	string GroupName;
	long   FunctionId;
	// Cough up the information about this function.
	const char* GetName() const { return Name.c_str(); }
	const char* GetType() const { return Type.c_str(); }
	const char* GetPrimaryGroup() const { return GroupName.c_str(); }
	long GetFunctionId() ;
	long GetCalls(int tid) { return NumCalls[tid]; }
	long GetSubrs(int tid) { return NumSubrs[tid]; }
	double GetExclTime(int tid) { return ExclTime[tid]; }
	double GetInclTime(int tid) { return InclTime[tid]; }
	TauGroup_t GetProfileGroup() const {return MyProfileGroup_; }
#ifdef PROFILE_STATS 
	double GetSumExclSqr(int tid) { return SumExclSqr[tid]; }
	void AddSumExclSqr(double ExclSqr, int tid) 
	  { SumExclSqr[tid] += ExclSqr; }
#endif // PROFILE_STATS 

private:
	TauGroup_t MyProfileGroup_;
};

// Global variables
vector<FunctionInfo*>& TheFunctionDB(void); 
int& TheSafeToDumpData(void);

//
// For efficiency, make the timing updates inline.
//
inline void 
FunctionInfo::ExcludeTime(double t, int tid)
{ // called by a function to decrease its parent functions time
	ExclTime[tid] -= t; // exclude from it the time spent in child function
}
	

inline void 
FunctionInfo::AddInclTime(double t, int tid)
{
  	InclTime[tid] += t; // Add Inclusive time
}

inline void
FunctionInfo::AddExclTime(double t, int tid)
{
 	ExclTime[tid] += t; // Add Total Time to Exclusive time (-ve)
}

inline void
FunctionInfo::IncrNumCalls(int tid)
{
	NumCalls[tid] ++; // Increment number of calls
} 


inline void
FunctionInfo::IncrNumSubrs(int tid)
{
  	NumSubrs[tid] ++;  // increment # of subroutines
}

inline void
FunctionInfo::SetAlreadyOnStack(bool value, int tid)
{
	AlreadyOnStack[tid] = value;
}

inline bool
FunctionInfo::GetAlreadyOnStack(int tid)
{
	return AlreadyOnStack[tid];
}
#endif /* _FUNCTIONINFO_H_ */
/***************************************************************************
 * $RCSfile: FunctionInfo.h,v $   $Author: sameer $
 * $Revision: 1.9 $   $Date: 1999/08/19 22:27:56 $
 * POOMA_VERSION_ID: $Id: FunctionInfo.h,v 1.9 1999/08/19 22:27:56 sameer Exp $ 
 ***************************************************************************/
