#define _BSD_SOURCE
#include <stdio.h>
#include <sstream>
#include <inttypes.h>
#include <omp.h>
#include <ompt.h>
#include <Profile/TauBfd.h>
#include <Profile/Profiler.h>
#include <tau_internal.h>
//#include "kmp.h"
#include <execinfo.h>
#ifdef OMPT_USE_LIBUNWIND
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#endif
// #include "ompt-signal.h"
#include <Profile/Profiler.h>
#include <Profile/TauEnv.h>

static bool initializing = false;
static bool initialized = false;
static bool tau_initialized = false;
#if defined (TAU_USE_TLS)
__thread bool is_master = false;
#elif defined (TAU_USE_DTLS)
__declspec(thread) bool is_master = false;
#elif defined (TAU_USE_PGS)
#include "pthread.h"
pthread_key_t thr_id_key;
#endif

int get_ompt_tid(void) {
#if defined (TAU_USE_TLS)
  if (is_master) return 0;
#elif defined (TAU_USE_DTLS)
  if (is_master) return 0;
#elif defined (TAU_USE_PGS)
  if (pthread_getspecific(thr_id_key) != NULL) return 0;
#endif
  return Tau_get_thread();
}

int Tau_set_tau_initialized() { tau_initialized = true; };

static const char* ompt_thread_type_t_values[] = {
  NULL,
  "ompt_thread_initial",
  "ompt_thread_worker",
  "ompt_thread_other"
};

static const char* ompt_task_status_t_values[] = {
  NULL,
  "ompt_task_complete",
  "ompt_task_yield",
  "ompt_task_cancel",
  "ompt_task_others"
};
static const char* ompt_cancel_flag_t_values[] = {
  "ompt_cancel_parallel",
  "ompt_cancel_sections",
  "ompt_cancel_do",
  "ompt_cancel_taskgroup",
  "ompt_cancel_activated",
  "ompt_cancel_detected",
  "ompt_cancel_discarded_task"
};

static void format_task_type(int type, char* buffer)
{
  char* progress = buffer;
  if(type & ompt_task_initial) progress += sprintf(progress, "ompt_task_initial");
  if(type & ompt_task_implicit) progress += sprintf(progress, "ompt_task_implicit");
  if(type & ompt_task_explicit) progress += sprintf(progress, "ompt_task_explicit");
  if(type & ompt_task_target) progress += sprintf(progress, "ompt_task_target");
  if(type & ompt_task_undeferred) progress += sprintf(progress, "|ompt_task_undeferred");
  if(type & ompt_task_untied) progress += sprintf(progress, "|ompt_task_untied");
  if(type & ompt_task_final) progress += sprintf(progress, "|ompt_task_final");
  if(type & ompt_task_mergeable) progress += sprintf(progress, "|ompt_task_mergeable");
  if(type & ompt_task_merged) progress += sprintf(progress, "|ompt_task_merged");
}

/* Function pointers.  These are all queried from the runtime during
 * ompt_initialize() */
static ompt_set_callback_t ompt_set_callback;
static ompt_get_task_info_t ompt_get_task_info;
static ompt_get_thread_data_t ompt_get_thread_data;
static ompt_get_parallel_info_t ompt_get_parallel_info;
static ompt_get_unique_id_t ompt_get_unique_id;
static ompt_get_num_places_t ompt_get_num_places;
static ompt_get_place_proc_ids_t ompt_get_place_proc_ids;
static ompt_get_place_num_t ompt_get_place_num;
static ompt_get_partition_place_nums_t ompt_get_partition_place_nums;
static ompt_get_proc_id_t ompt_get_proc_id;
static ompt_enumerate_states_t ompt_enumerate_states;
static ompt_enumerate_mutex_impls_t ompt_enumerate_mutex_impls;

/*Externs*/
extern "C" char* Tau_ompt_resolve_callsite_eagerly(unsigned long addr, char * resolved_address);

static void
on_ompt_callback_parallel_begin(
  ompt_data_t *parent_task_data,
  const ompt_frame_t *parent_task_frame,
  ompt_data_t* parallel_data,
  uint32_t requested_team_size,
  ompt_invoker_t invoker,
  const void *codeptr_ra)
{
  char timerName[1024];
  char resolved_address[1024];

  TauInternalFunctionGuard protects_this_function; 	
  if(codeptr_ra) {
      void * codeptr_ra_copy = (void*) codeptr_ra;
      unsigned long addr = Tau_convert_ptr_to_unsigned_long(codeptr_ra_copy);

      /*Resolve addresses at runtime in case the user really wants to pay the price of doing so.
       *Enabling eager resolving of addresses is only useful in situations where 
       *OpenMP routines are instrumented in shared libraries that get unloaded
       *before TAU has a chance to resolve addresses*/
      if (TauEnv_get_ompt_resolve_address_eagerly()) {
        Tau_ompt_resolve_callsite_eagerly(addr, resolved_address);
        sprintf(timerName, "OpenMP_Parallel_Region %s", resolved_address);
      } else {
        sprintf(timerName, "OpenMP_Parallel_Region ADDR <%lx>", addr);
      }

      void *handle = NULL;
      TAU_PROFILER_CREATE(handle, timerName, "", TAU_OPENMP);
      parallel_data->ptr = (void*)handle;
      TAU_PROFILER_START(handle); 
  }
}

static void
on_ompt_callback_parallel_end(
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  ompt_invoker_t invoker,
  const void *codeptr_ra)
{
  TauInternalFunctionGuard protects_this_function;

  if(codeptr_ra) {
    TAU_PROFILER_STOP(parallel_data->ptr);
  }
}

static void
on_ompt_callback_task_create(
    ompt_data_t *parent_task_data,     /* id of parent task            */
    const ompt_frame_t *parent_frame,  /* frame data for parent task   */
    ompt_data_t* new_task_data,        /* id of created task           */
    int type,
    int has_dependences,
    const void *codeptr_ra)               /* pointer to outlined function */
{
  char contextEventName[2058];
  char buffer[2048];
  TauInternalFunctionGuard protects_this_function; 	
  if(codeptr_ra) {
      void * codeptr_ra_copy = (void*) codeptr_ra;
      unsigned long addr = Tau_convert_ptr_to_unsigned_long(codeptr_ra_copy);
      format_task_type(type, buffer);

      /*TODO: Srinivasan: This does not really fit in as a context event. Just keeping this here 
       * for the time being. It makes no sense to calculate any statistics for such events. 
       * Nick's advice: The ThreadTaskCreate/ThreadTaskSwitch/ThreadTaskComplete events are used in OTF2 to indicate creation of a task, 
       * execution of a task, or completion of a task. The events are identified solely by a location and a uint64_t taskID. 
       * There’s no region associated with it, so there’s no way within that event type, as I can tell, to specify that the task 
       * corresponds to a particular region of code. 
       * Based on this paper about the task support in OTF2 <http://apps.fz-juelich.de/jsc-pubsystem/aigaion/attachments/schmidl_iwomp2012.pdf-5d909613b453c6fdbf34af237b8d5e52.pdf> 
       * it appears that these are supposed to be used in conjunction with Enter and Leave to assign code regions to the task. See Figure 1 in that paper.
       * I would recommend that you use Score-P to generate a trace using those event types and then look at the trace using otf2_print to figure out how it’s using the events.*/
      sprintf(contextEventName, "OpenMP_Task_Create %s ADDR <%lx> ", buffer, addr);

      TAU_REGISTER_CONTEXT_EVENT(event, contextEventName);
      TAU_EVENT_DISABLE_MAX(event);
      TAU_EVENT_DISABLE_MIN(event);
      TAU_EVENT_DISABLE_MEAN(event);
      TAU_EVENT_DISABLE_STDDEV(event);
      TAU_CONTEXT_EVENT(event, type);
  }
}

static void
on_ompt_callback_master(
  ompt_scope_endpoint_t endpoint,
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  const void *codeptr_ra)
{
  TauInternalFunctionGuard protects_this_function;
  char timerName[1024];
  char resolved_address[1024];
  void * codeptr_ra_copy;
  unsigned long addr;
  void *handle = NULL;

  if(codeptr_ra) {
    switch(endpoint)
    {
      case ompt_scope_begin:
        codeptr_ra_copy = (void*) codeptr_ra;
        addr = Tau_convert_ptr_to_unsigned_long(codeptr_ra_copy);
        /*Resolve addresses at runtime in case the user really wants to pay the price of doing so.
         *Enabling eager resolving of addresses is only useful in situations where 
         *OpenMP routines are instrumented in shared libraries that get unloaded
         *before TAU has a chance to resolve addresses*/
        if (TauEnv_get_ompt_resolve_address_eagerly()) {
          Tau_ompt_resolve_callsite_eagerly(addr, resolved_address);
          sprintf(timerName, "OpenMP_Parallel_Region %s", resolved_address);
        } else {
          sprintf(timerName, "OpenMP_Parallel_Region ADDR <%lx>", addr);
        }
        TAU_PROFILER_CREATE(handle, timerName, "", TAU_OPENMP);
        parallel_data->ptr = (void*)handle;
        TAU_PROFILER_START(handle); 
        break;
      case ompt_scope_end:
        TAU_PROFILER_STOP(parallel_data->ptr);
        break;
    }
  }
}

static void
on_ompt_callback_work(
  ompt_work_type_t wstype,
  ompt_scope_endpoint_t endpoint,
  ompt_data_t *parallel_data,
  ompt_data_t *task_data,
  uint64_t count,
  const void *codeptr_ra)
{
  TauInternalFunctionGuard protects_this_function;
  void *handle = NULL;
  char timerName[1024];
  char resolved_address[1024];
  if(codeptr_ra) {
    
   void * codeptr_ra_copy = (void*) codeptr_ra;
   unsigned long addr = Tau_convert_ptr_to_unsigned_long(codeptr_ra_copy);
    switch(endpoint)
    {
      case ompt_scope_begin:
        if(TauEnv_get_ompt_resolve_address_eagerly()) {
          Tau_ompt_resolve_callsite_eagerly(addr, resolved_address);
          switch(wstype)
          {
            case ompt_work_loop:
    	      sprintf(timerName, "OpenMP_Work_Loop %s", resolved_address);
              break;
            case ompt_work_sections:
              sprintf(timerName, "OpenMP_Work_Sections %s", resolved_address);
              break;
            case ompt_work_single_executor:
              sprintf(timerName, "OpenMP_Work_Single_Executor %s", resolved_address);
              break; /* WARNING: LLVM BUG ALERT - The ompt_scope_begin for this work type is triggered, but the corresponding ompt_scope_end is not triggered when using GNU to compile the tool code*/ 
            case ompt_work_single_other:
              sprintf(timerName, "OpenMP_Work_Single_Other %s", resolved_address);
              break;
            case ompt_work_workshare:
              sprintf(timerName, "OpenMP_Work_Workshare %s", resolved_address);
              break;
            case ompt_work_distribute:
              sprintf(timerName, "OpenMP_Work_Distribute %s", resolved_address);
              break;
            case ompt_work_taskloop:
              sprintf(timerName, "OpenMP_Work_Taskloop %s", resolved_address);
              break;
          }
        } else {
          switch(wstype)
          {
            case ompt_work_loop:
    	      sprintf(timerName, "OpenMP_Work_Loop ADDR <%lx>", addr);
              break;
            case ompt_work_sections:
              sprintf(timerName, "OpenMP_Work_Sections ADDR <%lx>", addr);
              break;
            case ompt_work_single_executor:
              sprintf(timerName, "OpenMP_Work_Single_Executor ADDR <%lx>", addr);
              break; /* WARNING: LLVM BUG ALERT - The ompt_scope_begin for this work type is triggered, but the corresponding ompt_scope_end is not triggered when using GNU to compile the tool code*/ 
            case ompt_work_single_other:
              sprintf(timerName, "OpenMP_Work_Single_Other ADDR <%lx>", addr);
              break;
            case ompt_work_workshare:
              sprintf(timerName, "OpenMP_Work_Workshare ADDR <%lx>", addr);
              break;
            case ompt_work_distribute:
              sprintf(timerName, "OpenMP_Work_Distribute ADDR <%lx>", addr);
              break;
            case ompt_work_taskloop:
              sprintf(timerName, "OpenMP_Work_Taskloop ADDR <%lx>", addr);
              break;

          }
        }
#ifndef __GNUG__ /*TODO: Remove this preprocessor check once the above bug with LLVM-GNU has been resolved.*/
        TAU_PROFILER_CREATE(handle, timerName, " ", TAU_OPENMP);
        TAU_PROFILER_START(handle);
        parallel_data->ptr = (void*)handle;
#endif
        break;
      case ompt_scope_end: 
#ifndef __GNUG__  /*TODO: Remove this preprocessor check once the above bug with LLVM-GNU has been resolved.*/
	TAU_PROFILER_STOP(parallel_data->ptr);
#endif
	break;
    }
  }
}

static void
on_ompt_callback_thread_begin(
  ompt_thread_type_t thread_type,
  ompt_data_t *thread_data)
{
  TauInternalFunctionGuard protects_this_function;
#if defined (TAU_USE_TLS)
  if (is_master) return; // master thread can't be a new worker.
#elif defined (TAU_USE_DTLS)
  if (is_master) return; // master thread can't be a new worker.
#elif defined (TAU_USE_PGS)
  if (pthread_getspecific(thr_id_key) != NULL) return; // master thread can't be a new worker.
#endif
  void *handle = NULL;
  char timerName[100];
  sprintf(timerName, "OpenMP_Thread_Type_%s", ompt_thread_type_t_values[thread_type]);
  TAU_PROFILER_CREATE(handle, timerName, "", TAU_DEFAULT);
  thread_data->ptr = (void*)handle;
  TAU_PROFILER_START(handle); 
}

static void
on_ompt_callback_thread_end(
  ompt_data_t *thread_data)
{
#if defined (TAU_USE_TLS)
  if (is_master) return; // master thread can't be a new worker.
#elif defined (TAU_USE_DTLS)
  if (is_master) return; // master thread can't be a new worker.
#elif defined (TAU_USE_PGS)
  if (pthread_getspecific(thr_id_key) != NULL) return; // master thread can't be a new worker.
#endif
  TauInternalFunctionGuard protects_this_function;
  TAU_PROFILER_STOP(thread_data->ptr);
}

static void
on_ompt_callback_implicit_task(
    ompt_scope_endpoint_t endpoint,
    ompt_data_t *parallel_data,
    ompt_data_t *task_data,
    unsigned int team_size,
    unsigned int thread_num)
{
  TauInternalFunctionGuard protects_this_function;
  const char *timerName= "OpenMP_Implicit_Task";

  TAU_PROFILE_TIMER(handle, timerName, "", TAU_DEFAULT);

  switch(endpoint)
  {
    case ompt_scope_begin:
      TAU_PROFILE_START(handle);
      break;
    case ompt_scope_end:
      TAU_PROFILE_STOP(handle);
      break;
  }
}

static void
on_ompt_callback_sync_region(
    ompt_sync_region_kind_t kind,
    ompt_scope_endpoint_t endpoint,
    ompt_data_t *parallel_data,
    ompt_data_t *task_data,
    const void *codeptr_ra)
{
  TauInternalFunctionGuard protects_this_function;
  void *handle = NULL;
  char timerName[1024];
  char resolved_address[1024];

  if(codeptr_ra) {
    void * codeptr_ra_copy = (void*) codeptr_ra;
    unsigned long addr = Tau_convert_ptr_to_unsigned_long(codeptr_ra_copy);

    switch(endpoint)
    {
      case ompt_scope_begin:
        if(TauEnv_get_ompt_resolve_address_eagerly()) {
          Tau_ompt_resolve_callsite_eagerly(addr, resolved_address);
          switch(kind)
          {
            case ompt_sync_region_barrier:
              sprintf(timerName, "OpenMP_Sync_Region_Barrier %s", resolved_address);
              break;
            case ompt_sync_region_taskwait:
              sprintf(timerName, "OpenMP_Sync_Region_Taskwait %s", resolved_address);
              break;
            case ompt_sync_region_taskgroup:
              sprintf(timerName, "OpenMP_Sync_Region_Taskgroup %s", resolved_address);
              break;
          }
        } else {
          switch(kind)
          {
            case ompt_sync_region_barrier:
              sprintf(timerName, "OpenMP_Sync_Region_Barrier ADDR <%lx>", addr);
              break;
            case ompt_sync_region_taskwait:
              sprintf(timerName, "OpenMP_Sync_Region_Taskwait ADDR <%lx>", addr);
              break;
            case ompt_sync_region_taskgroup:
              sprintf(timerName, "OpenMP_Sync_Region_Taskgroup ADDR <%lx>", addr);
              break;
          }
        }
        TAU_PROFILER_CREATE(handle, timerName, " ", TAU_OPENMP);
        TAU_PROFILER_START(handle);
        task_data->ptr = (void*)handle;
        break;
      case ompt_scope_end:
        TAU_PROFILER_STOP(task_data->ptr);
        break;
    }

  }
}

static void
on_ompt_callback_idle(
    ompt_scope_endpoint_t endpoint)
{
  TauInternalFunctionGuard protects_this_function;
  const char *timerName= "OpenMP_Idle";

  TAU_PROFILE_TIMER(handle, timerName, "", TAU_DEFAULT);

  switch(endpoint)
  {
    case ompt_scope_begin:
      TAU_PROFILE_START(handle);
      break;
    case ompt_scope_end:
      TAU_PROFILE_STOP(handle);
      break;
  }

  return;
}


inline static void register_callback(ompt_callbacks_t name, ompt_callback_t cb) {
  int ret = ompt_set_callback(name, cb);

  switch(ret) { 
    case ompt_set_never:
      fprintf(stderr, "TAU: WARNING: Callback for event %s could not be registered\n", name); 
      break; 
    case ompt_set_sometimes: 
      TAU_VERBOSE("TAU: Callback for event %s registered with return value %s\n", name, "ompt_set_sometimes");
      break;
    case ompt_set_sometimes_paired:
      TAU_VERBOSE("TAU: Callback for event %s registered with return value %s\n", name, "ompt_set_sometimes_paired");
      break;
    case ompt_set_always:
      TAU_VERBOSE("TAU: Callback for event %s registered with return value %s\n", name, "ompt_set_always");
      break;
  }
}

#define cb_t(name) (ompt_callback_t)&name
extern "C" int ompt_initialize(
  ompt_function_lookup_t lookup,
  ompt_data_t* tool_data)
{
  int ret;
  Tau_init_initializeTAU();
  if (initialized || initializing) return 0;
  initializing = true;
  TauInternalFunctionGuard protects_this_function;
  if (!TauEnv_get_openmp_runtime_enabled()) return 0;

#if defined (TAU_USE_TLS)
  is_master = true;
#elif defined (TAU_USE_DTLS)
  is_master = true;
#elif defined (TAU_USE_PGS)
  pthread_key_create(&thr_id_key, NULL);
  pthread_setspecific(thr_id_key, 1);
#endif


/* Gather the required function pointers using the lookup tool */
  TAU_VERBOSE("Registering OMPT events...\n"); fflush(stderr);
  ompt_set_callback = (ompt_set_callback_t) lookup("ompt_set_callback");
  ompt_get_task_info = (ompt_get_task_info_t) lookup("ompt_get_task_info");
  ompt_get_thread_data = (ompt_get_thread_data_t) lookup("ompt_get_thread_data");
  ompt_get_parallel_info = (ompt_get_parallel_info_t) lookup("ompt_get_parallel_info");
  ompt_get_unique_id = (ompt_get_unique_id_t) lookup("ompt_get_unique_id");

  ompt_get_num_places = (ompt_get_num_places_t) lookup("ompt_get_num_places");
  ompt_get_place_proc_ids = (ompt_get_place_proc_ids_t) lookup("ompt_get_place_proc_ids");
  ompt_get_place_num = (ompt_get_place_num_t) lookup("ompt_get_place_num");
  ompt_get_partition_place_nums = (ompt_get_partition_place_nums_t) lookup("ompt_get_partition_place_nums");
  ompt_get_proc_id = (ompt_get_proc_id_t) lookup("ompt_get_proc_id");
  ompt_enumerate_states = (ompt_enumerate_states_t) lookup("ompt_enumerate_states");
  ompt_enumerate_mutex_impls = (ompt_enumerate_mutex_impls_t) lookup("ompt_enumerate_mutex_impls");

/* Required events */

  register_callback(ompt_callback_parallel_begin, cb_t(on_ompt_callback_parallel_begin));
  register_callback(ompt_callback_parallel_end, cb_t(on_ompt_callback_parallel_end));
  register_callback(ompt_callback_task_create, cb_t(on_ompt_callback_task_create));
  #ifdef TAU_OMPT_ENABLE_FULL  //We should have different modes for OMPT support. The default mode should NOT enable high overhead callbacks.
    register_callback(ompt_callback_implicit_task, cb_t(on_ompt_callback_implicit_task)); /*Enabling this callback is a source of HUGE overheads */
  #endif
  register_callback(ompt_callback_thread_begin, cb_t(on_ompt_callback_thread_begin));
  register_callback(ompt_callback_thread_end, cb_t(on_ompt_callback_thread_end));

/* Optional events */

  register_callback(ompt_callback_work, cb_t(on_ompt_callback_work));
  register_callback(ompt_callback_master, cb_t(on_ompt_callback_master));

  // [JL]

  #ifdef TAU_OMPT_ENABLE_FULL /*High overhead*/
    register_callback(ompt_callback_sync_region, cb_t(on_ompt_callback_sync_region)); 
  #endif
  register_callback(ompt_callback_idle, cb_t(on_ompt_callback_idle)); // low overhead

  initialized = true;
  initializing = false;
  return 1; //success
}

extern "C" void ompt_finalize(ompt_data_t* tool_data)
{
  TAU_VERBOSE("OpenMP runtime is shutting down...\n");
}

extern "C" ompt_start_tool_result_t * ompt_start_tool(
  unsigned int omp_version,
  const char *runtime_version)
{
  static ompt_start_tool_result_t result;
  result.initialize = &ompt_initialize;
  result.finalize = &ompt_finalize;
  result.tool_data.value = 0L;
  result.tool_data.ptr = NULL;
  return &result;
}
