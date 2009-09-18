/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/tau	           **
*****************************************************************************
**    Copyright 2009  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/****************************************************************************
**	File 		: TauSampling.cpp  				   **
**	Description 	: TAU Profiling Package				   **
**	Contact		: tau-bugs@cs.uoregon.edu               	   **
**	Documentation	: See http://www.cs.uoregon.edu/research/tau       **
**                                                                         **
**      Description     : This file contains all the sampling related code **
**                                                                         **
****************************************************************************/


/****************************************************************************
 *
 *                      University of Illinois/NCSA
 *                          Open Source License
 *
 *          Copyright(C) 2004-2006, The Board of Trustees of the
 *              University of Illinois. All rights reserved.
 *
 *                             Developed by:
 *
 *                        The PerfSuite Project
 *            National Center for Supercomputing Applications
 *              University of Illinois at Urbana-Champaign
 *
 *                   http://perfsuite.ncsa.uiuc.edu/
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * + Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimers.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimers in
 *   the documentation and/or other materials provided with the distribution.
 * + Neither the names of The PerfSuite Project, NCSA/University of Illinois
 *   at Urbana-Champaign, nor the names of its contributors may be used to
 *   endorse or promote products derived from this Software without specific
 *   prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS WITH THE SOFTWARE.
 ****************************************************************************/



#ifdef TAU_EXP_SAMPLING

#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#include <TAU.h>
#include <Profile/TauMetrics.h>


typedef struct {
  int valid;
  caddr_t pc;
  x_uint64 timestamp;
  double counters[TAU_MAX_COUNTERS];
  x_uint64 deltaStart;
  x_uint64 deltaStop;
  int outputCallpath;
} TauSamplingRecord;



FILE *ebsTrace;
TauSamplingRecord theRecord;


#define USE_SIGCONTEXT 1

static inline caddr_t get_pc(void *p) {
  struct ucontext *uc = (struct ucontext *) p;
  caddr_t pc;
  struct sigcontext *sc;
  sc = (struct sigcontext *) &uc->uc_mcontext;
# ifdef __x86_64__
  pc = (caddr_t) sc->rip;
# elif IA32
  pc = (caddr_t) sc->eip;
# elif __ia64__
  pc = (caddr_t) sc->sc_ip;
# else
#  error "profile handler not defined for this architecture"
# endif

  return pc;
}




/*********************************************************************
 * Write out a single event record
 ********************************************************************/
void Tau_sampling_flush_record(TauSamplingRecord *record) {

  fprintf (ebsTrace, "%lld | ", record->timestamp);
  fprintf (ebsTrace, "%lld | ", record->deltaStart);
  fprintf (ebsTrace, "%lld | ", record->deltaStop);
  fprintf (ebsTrace, "%x | ", record->pc);

  for (int i=0; i<Tau_Global_numCounters; i++) {
    fprintf (ebsTrace, "%.16G ", record->counters[i]);
  }

  if (record->outputCallpath) {
    fprintf (ebsTrace, "| ");
    TAU_QUERY_DECLARE_EVENT(event);
    const char *str;
    TAU_QUERY_GET_CURRENT_EVENT(event);
    TAU_QUERY_GET_EVENT_NAME(event, str);
    
    
    int depth = TauEnv_get_callpath_depth();
    if (depth < 1) {
      depth = 1;
    }
    
    while (str && depth > 0) {
      //    printf ("inside %s\n", str);
      
      Profiler *p = (Profiler*)event;
      fprintf (ebsTrace, "%d", p->ThisFunction->GetFunctionId());
      TAU_QUERY_GET_PARENT_EVENT(event);
      TAU_QUERY_GET_EVENT_NAME(event, str);
      if (str) {
       //fprintf (ebsTrace, " : ", str);
	fprintf (ebsTrace, " ", str);
      }
      depth--;
    }
    // fprintf (ebsTrace, "");
    
    fprintf (ebsTrace, "\n");
  } else {
    fprintf (ebsTrace, "| -1\n");
  }
}

/*********************************************************************
 * Handler for event exit (stop)
 ********************************************************************/
int Tau_sampling_event_stop(double stopTime) {
  if (theRecord.valid == 0) {
    return 0;
  }
  int tid = RtsLayer::myThread();

  Profiler *profiler = TauInternal_CurrentProfiler(tid);
  double startTime = profiler->StartTime[0]; // gtod must be counter 0
  
  theRecord.deltaStart = (x_uint64) startTime;
  theRecord.deltaStop = (x_uint64) stopTime;
  theRecord.outputCallpath = 1;
  
  Tau_sampling_flush_record(&theRecord);
  theRecord.valid = 0;

  return 0;
}

/*********************************************************************
 * Handler for itimer interrupt
 ********************************************************************/
void Tau_sampling_handler(int signum, siginfo_t *si, void *p) {
  int tid = RtsLayer::myThread();

  if (theRecord.valid) {
    Tau_sampling_flush_record(&theRecord);
  }

  caddr_t pc;
  pc = get_pc(p);
//   printf ("sample on %x\n", pc);

  struct timeval tp;
  gettimeofday (&tp, 0);
  x_uint64 timestamp = ((x_uint64)tp.tv_sec * (x_uint64)1e6 + (x_uint64)tp.tv_usec);


  theRecord.timestamp = timestamp;
  theRecord.pc = pc;
  theRecord.deltaStart = 0;
  theRecord.deltaStop = 0;
  theRecord.outputCallpath = 0;
  theRecord.valid = 1;

  double values[TAU_MAX_COUNTERS];
  TauMetrics_getMetrics(tid, values);
  for (int i=0; i<Tau_Global_numCounters; i++) {
    theRecord.counters[i] = values[i];
  }

}



int Tau_sampling_outputHeader() {
  fprintf (ebsTrace, "# Format:\n");
  fprintf (ebsTrace, "# <timestamp> | <delta-begin> | <delta-end> | <pc> | <metric 1> ... <metric N> | <tau callpath>\n");
  fprintf (ebsTrace, "# Metrics:");
  for (int i=0; i<Tau_Global_numCounters; i++) {
    const char *name = TauMetrics_getMetricName(i);
    fprintf (ebsTrace, " %s", name);
  }
  fprintf (ebsTrace, "\n");
  char buffer[4096];
  bzero(buffer, 4096);
  int rc = readlink("/proc/self/exe", buffer, 4096);
  fprintf (ebsTrace, "# exe: %s\n", buffer);
}

/*********************************************************************
 * Initialize the sampling trace system
 ********************************************************************/
int Tau_sampling_init() {
  int ret;

  static struct itimerval itval;

  int threshold = 1000;

  itval.it_interval.tv_usec = itval.it_value.tv_usec = 1000 % 1000000;
  itval.it_interval.tv_sec =  itval.it_value.tv_sec = 1000 / 1000000;

  theRecord.valid = 0;
  theRecord.outputCallpath = 0;

  const char *profiledir = TauEnv_get_profiledir();

  char filename[4096];

  int tid = RtsLayer::myThread();
  int node = RtsLayer::myNode();

  node = 0;
  sprintf(filename,"%s/ebstrace.%d.%d.%d", profiledir, node, RtsLayer::myContext(), tid);

  ebsTrace = fopen(filename, "w");
  
  Tau_sampling_outputHeader();



  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  act.sa_sigaction = Tau_sampling_handler;
  act.sa_flags     = SA_SIGINFO;

  ret = sigaction(SIGALRM, &act, NULL);

  ret = setitimer(ITIMER_REAL, &itval, 0);
  if (ret != 0)  {
    printf ("TAU: Sampling error: %s\n", strerror(ret));
    return 0;
  }
  
  return 0;
}

/*********************************************************************
 * Finalize the sampling trace system
 ********************************************************************/
int Tau_sampling_finalize() {

  /* Disable sampling first */
  struct itimerval itval;
  int ret;
  
  itval.it_interval.tv_usec = itval.it_value.tv_usec =
    itval.it_interval.tv_sec = itval.it_value.tv_sec = 0;
  
  ret = setitimer(ITIMER_REAL, &itval, 0);
  if (ret != 0) {
    /* ERROR */
  }

  const char *profiledir = TauEnv_get_profiledir();
  char filename[4096];
  int tid = RtsLayer::myThread();
  int node = RtsLayer::myNode();
  node = 0;
  sprintf(filename,"%s/ebstracedef.%d.%d.%d", profiledir, node, RtsLayer::myContext(), tid);

  FILE *def = fopen(filename, "w");

  fprintf (def, "# Format:\n");
  fprintf (def, "# <id> | <name>\n");

  for (vector<FunctionInfo*>::iterator it = TheFunctionDB().begin(); it != TheFunctionDB().end(); it++) {
    FunctionInfo *fi = *it;
    fprintf (def,"%d | %s %s\n", fi->GetFunctionId(), fi->GetName(), fi->GetType());
  }
  fclose(def);

  Tau_sampling_outputHeader();

}


#endif /* TAU_EXP_SAMPLING */
