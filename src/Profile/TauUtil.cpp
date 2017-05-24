/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/tau	           **
*****************************************************************************
**    Copyright 2010                                                       **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/****************************************************************************
**	File            : TauUtil.cpp                                      **
**	Contact		: tau-bugs@cs.uoregon.edu                          **
**	Documentation	: See http://tau.uoregon.edu                       **
**                                                                         **
**      Description     : This file contains utility routines              **
**                                                                         **
****************************************************************************/

#include <TauUtil.h>
#include <TauPlugin.h>
#include <string>
#include <TauEnv.h>
#include <TauPluginInternals.h>
#include <stdarg.h>
#include <string.h>


#ifndef TAU_WINDOWS
#include <dlfcn.h>
#endif /* TAU_WINDOWS */

#define TAU_NAME_LENGTH 1024

/*********************************************************************
 * Abort execution with a message
 ********************************************************************/
void TAU_ABORT(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(EXIT_FAILURE);
}



/*********************************************************************
 * Create an buffer output device
 ********************************************************************/
Tau_util_outputDevice *Tau_util_createBufferOutputDevice() 
{
  Tau_util_outputDevice *out = (Tau_util_outputDevice*) TAU_UTIL_MALLOC (sizeof(Tau_util_outputDevice));
  if (out == NULL) {
    return NULL;
  }
  out->type = TAU_UTIL_OUTPUT_BUFFER;
  out->bufidx = 0;
  out->buflen = TAU_UTIL_INITIAL_BUFFER;
  out->buffer = (char *)malloc(out->buflen + 1);
  return out;
}

/*********************************************************************
 * Return output buffer
 ********************************************************************/
char *Tau_util_getOutputBuffer(Tau_util_outputDevice *out) {
  return out->buffer;
}

/*********************************************************************
 * Return output buffer length
 ********************************************************************/
int Tau_util_getOutputBufferLength(Tau_util_outputDevice *out) {
  return out->bufidx;
}

/*********************************************************************
 * Free and close output device
 ********************************************************************/
void Tau_util_destroyOutputDevice(Tau_util_outputDevice *out) {
  if (out->type == TAU_UTIL_OUTPUT_BUFFER) {
    free (out->buffer);
  } else {
    fclose(out->fp);
  }
  free (out);
}

/*********************************************************************
 * Write to output device
 ********************************************************************/
int Tau_util_output(Tau_util_outputDevice *out, const char *format, ...) {
  int rs;
  va_list args;
  if (out->type == TAU_UTIL_OUTPUT_BUFFER) {
    va_start(args, format);
    rs = vsprintf(out->buffer+out->bufidx, format, args);
    va_end(args);
    out->bufidx+=rs;
    if (out->bufidx+TAU_UTIL_OUTPUT_THRESHOLD > out->buflen) {
      out->buflen = out->buflen * 2;
      out->buffer = (char*) realloc (out->buffer, out->buflen);
    }
  } else {
    va_start(args, format);
    rs = vfprintf(out->fp, format, args);
    va_end(args);
  }
  return rs;
}

/*********************************************************************
 * Read an entire line from a file
 ********************************************************************/
int Tau_util_readFullLine(char *line, FILE *fp) {
  int ch;
  int i = 0; 
  while ( (ch = fgetc(fp)) && ch != EOF && ch != (int) '\n') {
    line[i++] = (unsigned char) ch;
  }
  // Be careful to check that line is large enough:
  // sizeof(line) == strlen(str) + 1
  line[i] = '\0'; 
  return i; 
}

/*********************************************************************
 * Duplicates a string and replaces all the runs of spaces with a 
 * single space.
 ********************************************************************/
char const * Tau_util_removeRuns(char const * spaced_str) 
{
  if (!spaced_str) {
    return spaced_str; /* do nothing with a null string */
  }

  // Skip over spaces at start of string
  while (*spaced_str && *spaced_str == ' ') {
    ++spaced_str;
  }

  // String copy
  int len = strlen(spaced_str);
  char * str = (char *)malloc(len+1);

  // Copy from spaced_str ignoring runs of multiple spaces
  char c;
  char * dst = str;
  char const * src = spaced_str;
  char const * end = spaced_str + len;
  while ((c = *src) && src < end) {
    ++src;
    *dst = c;
    ++dst;
    if(c == ' ')
      while(*src == ' ')
        ++src;
  }
  *dst = '\0';

  return str;
}


void *Tau_util_malloc(size_t size, const char *file, int line) {
  void *ptr = malloc (size);
  if (!ptr) {
    TAU_ABORT("TAU: Abort: Unable to allocate memory (malloc) at %s:%d\n", file, line);
  }
  return ptr;
}

void *Tau_util_calloc(size_t size, const char *file, int line) {
  void *ptr = calloc (1,size);
  if (!ptr) {
    TAU_ABORT("TAU: Abort: Unable to allocate memory (calloc) at %s:%d\n", file, line);
  }
  return ptr;
}

/*********************************************************************
 * Create and return a new plugin manager if plugin system is un-initialized.
 * If it is already initialized, return a reference to the same plugin manager - Singleton Pattern
 ********************************************************************/
PluginManager* Tau_util_get_plugin_manager() {
  static PluginManager * plugin_manager = NULL;
  static int is_plugin_system_initialized = 0;
  
  /*Allocate memory for the plugin list and callback list*/
  if(!is_plugin_system_initialized) {

    plugin_manager = (PluginManager*)malloc(sizeof(PluginManager));
    plugin_manager->plugin_list = (Tau_plugin_list *)malloc(sizeof(Tau_plugin_list));
    (plugin_manager->plugin_list)->head = NULL;

    plugin_manager->callback_list = (Tau_plugin_callback_list*)malloc(sizeof(Tau_plugin_callback_list));
    (plugin_manager->callback_list)->head = NULL;
    is_plugin_system_initialized = 1;
  }

  return plugin_manager;
}

/*********************************************************************
 * Initializes the plugin system by loading and registering all plugins
 ********************************************************************/
int Tau_initialize_plugin_system() {
  return(Tau_util_load_and_register_plugins(Tau_util_get_plugin_manager()));
}

/*********************************************************************
 * Internal function that helps parse a token for the plugin name
 ********************************************************************/
int Tau_util_parse_plugin_token(char * token, char ** plugin_name, char *** plugin_args, int * plugin_num_args) {
  int length_of_arg_string = -1;
  char * save_ptr;
  char * arg_string;
  char * arg_token;
  char *pos_left = NULL;
  char *pos_right = NULL;
  

  *plugin_num_args = 0;
  *plugin_name = (char*)malloc(1024*sizeof(char));
  pos_left = strchr(token, '(');
  pos_right = strchr(token, ')');

  if(pos_left == NULL && pos_right == NULL) {
    strcpy(*plugin_name, token);
    return 0;
  } else if (pos_left == NULL || pos_right == NULL) {
    return -1; //Bad plugin name
  }

  *plugin_args = (char**)malloc(10*sizeof(char*)); //Maximum of 10 args supported for now
  arg_string = (char*)malloc(1024*sizeof(char));

  length_of_arg_string = (pos_right - pos_left) - 1;

  strncpy(arg_string, pos_left+1, length_of_arg_string);
  strncpy(*plugin_name, token, (pos_left-token));

  arg_token = strtok_r(arg_string, ",", &save_ptr);

  int i = 0;
  /*Grab and pack, and count all the arguments to the plugin*/
  while(arg_token != NULL) {
    (*plugin_num_args)++;
    (*plugin_args)[i] = (char*)malloc(1024*sizeof(char));
    strcpy((*plugin_args)[i], arg_token);
    arg_token = strtok_r(NULL, ",", &save_ptr);
    i++;
  }

  TAU_VERBOSE("TAU PLUGIN: Arg string and count for token %s are %s and %d\n", token, arg_string, *plugin_num_args);

  return 0;
}


/********************************************************************* 
 * Load a list of plugins at TAU init, given following environment variables:
 *  - TAU_PLUGINS_NAMES
 *  - TAU_PLUGINS_PATH
********************************************************************* */
int Tau_util_load_and_register_plugins(PluginManager* plugin_manager)
{
  char pluginpath[1024];
  char listpluginsnames[1024];
  char *fullpath = NULL;
  char *token = NULL;
  char *plugin_name = NULL;
  char *initFuncName = NULL;
  char **plugin_args;
  char *save_ptr;
  int plugin_num_args;

  if((TauEnv_get_plugins_path() == NULL) || (TauEnv_get_plugins() == NULL)) {
    printf("TAU: One or more of the environment variable(s) TAU_PLUGINS_PATH: %s, TAU_PLUGINS: %s are empty\n", TauEnv_get_plugins_path(), TauEnv_get_plugins());
    return -1;
  }
  
  strcpy(pluginpath, TauEnv_get_plugins_path());
  strcpy(listpluginsnames, TauEnv_get_plugins());

  /*Individual plugin names are separated by a ":"*/
  token = strtok_r(listpluginsnames,":", &save_ptr); 
  TAU_VERBOSE("TAU: Trying to load plugin with name %s\n", token);

  fullpath = (char*)calloc(TAU_NAME_LENGTH, sizeof(char));

  while(token != NULL)
  {
    TAU_VERBOSE("TAU: Loading plugin: %s\n", token);
    strcpy(fullpath, "");
    strcpy(fullpath,pluginpath);
    if (Tau_util_parse_plugin_token(token, &plugin_name, &plugin_args, &plugin_num_args)) {
      printf("TAU: Plugin name specification does not match form <plugin_name1>(<plugin_arg1>,<plugin_arg2>):<plugin_name2>(<plugin_arg1>,<plugin_arg2>) for: %s\n",token);
      return -1;
    }

    strcat(fullpath, plugin_name);
    TAU_VERBOSE("TAU: Full path for the current plugin: %s\n", fullpath);
   
    /*Return a handle to the loaded dynamic object*/
    void* handle = Tau_util_load_plugin(plugin_name, fullpath, plugin_manager);

    if (handle) {
      /*If handle is NOT NULL, register the plugin's handlers for various supported events*/
      handle = Tau_util_register_plugin(plugin_name, plugin_args, plugin_num_args, handle, plugin_manager);
     
      /*Plugin registration failed. Bail*/
      if(!handle) return -1;
      TAU_VERBOSE("TAU: Successfully called the init func of plugin: %s\n", token);

    } else {
      /*Plugin loading failed for some reason*/
      return -1;
    }

    token = strtok_r(NULL, ":", &save_ptr);
  }

  free(fullpath);
  return 0;
}

/**************************************************************************************************************************
 * Use dlsym to find a function : TAU_PLUGIN_INIT_FUNC that the plugin MUST implement in order to register itself.
 * If plugin registration succeeds, then the callbacks for that plugin have been added to the plugin manager's callback list
 * ************************************************************************************************************************/
void* Tau_util_register_plugin(const char *name, char **args, int num_args, void* handle, PluginManager* plugin_manager) {
  PluginInitFunc init_func = (PluginInitFunc) dlsym(handle, TAU_PLUGIN_INIT_FUNC);

  if(!init_func) {
    printf("TAU: Failed to retrieve TAU_PLUGIN_INIT_FUNC from plugin %s with error:%s\n", name, dlerror());
    dlclose(handle); //TODO : Replace with Tau_plugin_cleanup();
    return NULL;
  }

  int return_val = init_func(num_args, args);
  if(return_val < 0) {
    printf("TAU: Call to init func for plugin %s returned failure error code %d\n", name, return_val);
    dlclose(handle); //TODO : Replace with Tau_plugin_cleanup();
    return NULL;
  } 
  return handle;
}

/**************************************************************************************************************************
 * Given a plugin name and fullpath, load a plugin and return a handle to the opened DSO
 * ************************************************************************************************************************/
void* Tau_util_load_plugin(const char *name, const char *path, PluginManager* plugin_manager) {
  void* handle = dlopen(path, RTLD_NOW);
  
  if (handle) {
    Tau_plugin * plugin = (Tau_plugin *)malloc(sizeof(Tau_plugin));
    strcpy(plugin->plugin_name, name);
    plugin->handle = handle;
    plugin->next = (plugin_manager->plugin_list)->head;
    (plugin_manager->plugin_list)->head = plugin;

    TAU_VERBOSE("TAU: Successfully loaded plugin: %s\n", name);
    return handle;    
  } else {
    printf("TAU: Failed loading %s plugin with error: %s\n", name, dlerror());
    return NULL;
  }
}

/**************************************************************************************************************************
 * Initialize Tau_plugin_callbacks structure with default values
 * This is necessary in order to prevent future event additions to affect older plugins
 * ************************************************************************************************************************/
extern "C" void Tau_util_init_tau_plugin_callbacks(Tau_plugin_callbacks * cb) {
  cb->FunctionRegistrationComplete = 0;
  cb->AtomicEventRegistrationComplete = 0;
  cb->AtomicEventTrigger = 0;
  cb->EndOfExecution = 0;
  cb->InterruptTrigger = 0;
}

/**************************************************************************************************************************
 * Helper function that makes a copy of all callbacks for events
 ***************************************************************************************************************************/
void Tau_util_make_callback_copy(Tau_plugin_callbacks * dest, Tau_plugin_callbacks * src) {
  dest->FunctionRegistrationComplete = src->FunctionRegistrationComplete;
  dest->AtomicEventTrigger = src->AtomicEventTrigger;
  dest->AtomicEventRegistrationComplete = src->AtomicEventRegistrationComplete;
  dest->EndOfExecution = src->EndOfExecution;
  dest->InterruptTrigger = src->InterruptTrigger;
}


/**************************************************************************************************************************
 * Register callbacks associated with well defined events defined in struct Tau_plugin_callbacks
 **************************************************************************************************************************/
extern "C" void Tau_util_plugin_register_callbacks(Tau_plugin_callbacks * cb) {
  PluginManager* plugin_manager = Tau_util_get_plugin_manager();

  Tau_plugin_callback_ * callback = (Tau_plugin_callback_ *)malloc(sizeof(Tau_plugin_callback_));
  Tau_util_make_callback_copy(&(callback->cb), cb);
  callback->next = (plugin_manager->callback_list)->head;
  (plugin_manager->callback_list)->head = callback;
}


/**************************************************************************************************************************
 * Overloaded function that invokes all registered callbacks for the function registration event
 ***************************************************************************************************************************/
void Tau_util_invoke_callbacks_(Tau_plugin_event_function_registration_data data) {
  PluginManager* plugin_manager = Tau_util_get_plugin_manager();
  Tau_plugin_callback_list * callback_list = plugin_manager->callback_list;
  Tau_plugin_callback_ * callback = callback_list->head;

  while(callback != NULL) {
   if(callback->cb.FunctionRegistrationComplete != 0) {
     callback->cb.FunctionRegistrationComplete(data);
   }
   callback = callback->next;
  }
}

/**************************************************************************************************************************
 * Overloaded function that invokes all registered callbacks for the atomic event registration event
 ****************************************************************************************************************************/
void Tau_util_invoke_callbacks_(Tau_plugin_event_atomic_event_registration_data data) {
  PluginManager* plugin_manager = Tau_util_get_plugin_manager();
  Tau_plugin_callback_list * callback_list = plugin_manager->callback_list;
  Tau_plugin_callback_ * callback = callback_list->head;

  while(callback != NULL) {
   if(callback->cb.AtomicEventRegistrationComplete != 0) {
     callback->cb.AtomicEventRegistrationComplete(data);
   }
   callback = callback->next;
  }
}

/**************************************************************************************************************************
 * Overloaded function that invokes all registered callbacks for the atomic event trigger event
 *****************************************************************************************************************************/
void Tau_util_invoke_callbacks_(Tau_plugin_event_atomic_event_trigger_data data) {
  PluginManager* plugin_manager = Tau_util_get_plugin_manager();
  Tau_plugin_callback_list * callback_list = plugin_manager->callback_list;
  Tau_plugin_callback_ * callback = callback_list->head;

  while(callback != NULL) {
   if(callback->cb.AtomicEventTrigger != 0) {
     callback->cb.AtomicEventTrigger(data);
   }
   callback = callback->next;
  }
}

/**************************************************************************************************************************
 * Overloaded function that invokes all registered callbacks for the end of execution event
 ******************************************************************************************************************************/
void Tau_util_invoke_callbacks_(Tau_plugin_event_end_of_execution_data data) {
  PluginManager* plugin_manager = Tau_util_get_plugin_manager();
  Tau_plugin_callback_list * callback_list = plugin_manager->callback_list;
  Tau_plugin_callback_ * callback = callback_list->head;

  while(callback != NULL) {
   if(callback->cb.EndOfExecution != 0) {
     callback->cb.EndOfExecution(data);
   }
   callback = callback->next;
  }


}

/**************************************************************************************************************************
 *  Overloaded function that invokes all registered callbacks for interrupt trigger event
 *******************************************************************************************************************************/
void Tau_util_invoke_callbacks_(Tau_plugin_event_interrupt_trigger_data data) {
  PluginManager* plugin_manager = Tau_util_get_plugin_manager();
  Tau_plugin_callback_list * callback_list = plugin_manager->callback_list;
  Tau_plugin_callback_ * callback = callback_list->head;

  while(callback != NULL) {
   if(callback->cb.InterruptTrigger != 0) {
     callback->cb.InterruptTrigger(data);
   }
   callback = callback->next;
  }
}

/*****************************************************************************************************************************
 * Wrapper function that calls the actual callback invocation function based on the event type
 ******************************************************************************************************************************/
extern "C" void Tau_util_invoke_callbacks(Tau_plugin_event event, const void * data) {

  switch(event) {
    case TAU_PLUGIN_EVENT_FUNCTION_REGISTRATION: {
      Tau_util_invoke_callbacks_(*(Tau_plugin_event_function_registration_data*)data);
      break;
    } 
    case TAU_PLUGIN_EVENT_ATOMIC_EVENT_REGISTRATION: {
      Tau_util_invoke_callbacks_(*(Tau_plugin_event_atomic_event_registration_data*)data);
      break;
    } 

    case TAU_PLUGIN_EVENT_ATOMIC_EVENT_TRIGGER: {
      Tau_util_invoke_callbacks_(*(Tau_plugin_event_atomic_event_trigger_data*)data);
      break;
    } 
    case TAU_PLUGIN_EVENT_END_OF_EXECUTION: {
      Tau_util_invoke_callbacks_(*(Tau_plugin_event_end_of_execution_data*)data);
      break;
    } 
    case TAU_PLUGIN_EVENT_INTERRUPT_TRIGGER: {
      Tau_util_invoke_callbacks_(*(Tau_plugin_event_interrupt_trigger_data*)data);
      break;
    }
  }
}

/*****************************************************************************************************************************
 * Clean up all plugins by closing all opened dynamic libraries and free associated structures
 *******************************************************************************************************************************/
int Tau_util_cleanup_all_plugins() {

  PluginManager* plugin_manager = Tau_util_get_plugin_manager();
  
  Tau_plugin * temp_plugin;
  Tau_plugin_callback_ * temp_callback;

  Tau_plugin * plugin = (plugin_manager->plugin_list)->head;
  Tau_plugin_callback_ * callback = (plugin_manager->callback_list)->head;

  /*Two separate while loops to handle the weird case that a plugin is loaded but doesn't register anything*/ 
  while(plugin) {
    temp_plugin = plugin;

    plugin = temp_plugin->next;

    /*Close the dynamic library*/
    if(temp_plugin->handle)
      dlclose(temp_plugin->handle);

    temp_plugin->next = NULL;

    free(temp_plugin);
  }   

  while(callback) {
    temp_callback = callback;
    callback = temp_callback->next;
    temp_callback->next = NULL;

    free(temp_callback);
  }

  return 0;
}


