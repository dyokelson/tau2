/****************************************************************************
**			TAU Portable Profiling Package			   **
**			http://www.cs.uoregon.edu/research/tau	           **
*****************************************************************************
**    Copyright 2010  						   	   **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
****************************************************************************/
/****************************************************************************
**	File 		: TauMetaData.h  				   **
**	Description 	: TAU Profiling Package				   **
**	Contact		: tau-bugs@cs.uoregon.edu               	   **
**	Documentation	: See http://www.cs.uoregon.edu/research/tau       **
**                                                                         **
**      Description     : This file contains metadata related routines     **
**                                                                         **
****************************************************************************/


#ifndef _TAU_METADATA_H_
#define _TAU_METADATA_H_

#include <TauMetaDataTypes.h>
#include <TauUtil.h>
#include <map>
#include <string.h>
#include <sstream>
using namespace std;

// the actual metadata key structure, can be nested.
class Tau_metadata_key {
  public:
  char* name;
  char* timer_context;
  int call_number;
  x_uint64 timestamp;
  Tau_metadata_key() {
    name = NULL;
    timer_context = NULL;
    call_number = 0;
    timestamp = 0;
  }
};

struct Tau_Metadata_Compare: std::binary_function<Tau_metadata_key,Tau_metadata_key,bool>
{
  bool operator()(const Tau_metadata_key& lhs, const Tau_metadata_key& rhs) const { 
    stringstream left;
    stringstream right;
	// what happens if timer_context is null? I guess it works...
	left << lhs.name << lhs.timer_context << lhs.call_number << ":" << lhs.timestamp;
	right << rhs.name << rhs.timer_context << rhs.call_number << ":" << rhs.timestamp;
    if (strcmp(left.str().c_str(), right.str().c_str()) < 0) return true;
	return false;
  }
};

map<Tau_metadata_key,Tau_metadata_value_t*,Tau_Metadata_Compare> &Tau_metadata_getMetaData(int tid);
int Tau_metadata_writeMetaData(Tau_util_outputDevice *out, int counter, int tid);
int Tau_metadata_writeMetaData(FILE *fp, int counter, int tid);
int Tau_metadata_writeMetaData(Tau_util_outputDevice *out, int tid);

int Tau_metadata_fillMetaData();
Tau_util_outputDevice *Tau_metadata_generateMergeBuffer();
void Tau_metadata_removeDuplicates(char *buffer, int buflen);

void Tau_metadata_register(const char *name, int value);
int Tau_metadata_mergeMetaData();

#endif /* _TAU_METADATA_H_ */
