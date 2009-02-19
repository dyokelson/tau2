/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/tau	           **
*****************************************************************************
**    Copyright 2008  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
**    Forschungszentrum Juelich                                            **
****************************************************************************/
/****************************************************************************
**	File 		: TauInit.cpp 			        	   **
**	Description 	: TAU Profiling Package				   **
**	Author		: Alan Morris					   **
**	Contact		: tau-bugs@cs.uoregon.edu               	   **
**	Documentation	: See http://www.cs.uoregon.edu/research/tau       **
**                                                                         **
**      Description     : TAU initialization                               **
**                                                                         **
****************************************************************************/

#include <Profile/TauEnv.h>
#include <TAU.h>


bool Tau_snapshot_initialization();
extern "C" void Tau_stack_initialization();
extern "C" int Tau_compensate_initialization();

extern "C" int InitializeTAU() {
  static bool initialized = false;
  if (initialized) {
    return 0;
  }
  
  // initialize environment variables
  TauEnv_initialize();
  
  // we need the timestamp of the "start"
  Tau_snapshot_initialization();
  
  // other initialization code should go here

#ifdef TAU_MULTIPLE_COUNTERS
  MultipleCounterLayer::initializeMultiCounterLayer();
#endif

  Tau_stack_initialization();

#ifdef TAU_COMPENSATE
  Tau_compensate_initialization();
#endif /* TAU_COMPENSATE */


  initialized = true;
  return 0;
}
