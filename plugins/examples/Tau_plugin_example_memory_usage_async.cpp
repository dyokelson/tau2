/************************************************************************************************
 * *   Plugin Testing
 * *   Tests basic functionality of a plugin for function registration event
 * *
 * *********************************************************************************************/

#include <TAU.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>

#include <Profile/TauEnv.h>
#include <Profile/TauMetrics.h>
#include <Profile/TauCollate.h>
#include <Profile/TauUtil.h>
#include <Profile/TauXML.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>

#ifdef TAU_MPI
#include <mpi.h>
#endif

#include <list>
#include <vector>

#include <Profile/TauPlugin.h>
#include <Profile/TauMemory.h>

#include <Profile/TauTrace.h>
#include <sys/resource.h>

#include <sys/types.h>
#include <unistd.h>

using namespace std;
std::vector<std::thread> thread_vec;
extern "C" int Tau_open_system_file(const char *filename);
extern "C" int Tau_read_load_event(int fd, double *value);

pthread_t tid1;

int done = 0; 

int load_id, usage_id;

static x_uint64 getTimeStamp()
{
  x_uint64 timestamp;
#ifdef TAU_WINDOWS
  timestamp = TauWindowsUsecD();
#else
  struct timeval tp;
  gettimeofday(&tp, 0);
  timestamp = (x_uint64)tp.tv_sec * (x_uint64)1e6 + (x_uint64)tp.tv_usec;
#endif
  return timestamp;
}

int Tau_plugin_event_pre_end_of_execution(Tau_plugin_event_pre_end_of_execution_data_t *data) {

  done = 1;

  int ret = pthread_join(tid1, NULL);

  fprintf(stderr, "Asynchronous plugin exiting...\n");

  return 0;
}

void * Tau_plugin_do_work(void * data) {
  double value = 0;
  static int fd = Tau_open_system_file("/proc/loadavg");

/*  RtsLayer::LockDB();

  for (int tid = 0; tid < RtsLayer::getTotalThreads(); tid++) {
    TauTraceInit(tid);
  }

  RtsLayer::UnLockDB();
*/

  while(!done) {
      value = 0;
      if (fd) {
        Tau_read_load_event(fd, &value);
    
       //Do not bother with recording the load if TAU is uninitialized. 
        if (Tau_init_check_initialized()) {
            value = value*100;
        } else {
          value = 0;
        }
      }
      struct rusage r_usage;
      x_uint64 ts = getTimeStamp();
      getrusage(RUSAGE_SELF,&r_usage);
      TauTraceEvent(load_id, (x_uint64)value, Tau_get_thread(), (x_uint64)ts, 1, TAU_TRACE_EVENT_KIND_USEREVENT); 
      TauTraceEvent(usage_id, (x_uint64)r_usage.ru_maxrss, Tau_get_thread(), (x_uint64)ts, 1, TAU_TRACE_EVENT_KIND_USEREVENT); 

      fprintf(stderr, "Load and Max Memory usage = %lf, %ld\n", value, r_usage.ru_maxrss);
      sleep(2);
  }

 /* RtsLayer::LockDB();

  for (int tid = 0; tid < RtsLayer::getTotalThreads(); tid++) {
    TauTraceClose(tid);
  }

  RtsLayer::UnLockDB();
*/

}

/*This is the init function that gets invoked by the plugin mechanism inside TAU.
 * Every plugin MUST implement this function to register callbacks for various events 
 * that the plugin is interested in listening to*/
extern "C" int Tau_plugin_init_func(int argc, char **argv, int id) {

  Tau_plugin_callbacks * cb = (Tau_plugin_callbacks*)malloc(sizeof(Tau_plugin_callbacks));
  TAU_UTIL_INIT_TAU_PLUGIN_CALLBACKS(cb);
  cb->PreEndOfExecution = Tau_plugin_event_pre_end_of_execution;

  load_id = RtsLayer::GenerateUniqueId();
  usage_id = RtsLayer::GenerateUniqueId();

  TAU_UTIL_PLUGIN_REGISTER_CALLBACKS(cb, id);
  void * data = NULL;

  int ret = pthread_create(&tid1, NULL, Tau_plugin_do_work, NULL);

  //thread_vec.push_back(std::thread(Tau_plugin_do_work, data));

  return 0;
}

