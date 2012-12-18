/****************************************************************************
**                      TAU Portable Profiling Package                     **
**                      http://tau.uoregon.edu                             **
*****************************************************************************
**    Copyright 2009                                                       **
**    Department of Computer and Information Science, University of Oregon **
**    Advanced Computing Laboratory, Los Alamos National Laboratory        **
**    Forschungszentrum Juelich                                            **
****************************************************************************/
/****************************************************************************
**      File            : TauMemory.h                                      **
**      Contact         : tau-bugs@cs.uoregon.edu                          **
**      Documentation   : See http://tau.uoregon.edu                       **
**      Description     : Support for memory tracking                      **
**                                                                         **
****************************************************************************/

//////////////////////////////////////////////////////////////////////
// Include Files 
//////////////////////////////////////////////////////////////////////

#ifndef _TAU_MEMORY_H_
#define _TAU_MEMORY_H_


#if defined(__darwin__) || defined(__APPLE__) || defined(TAU_XLC)
#undef HAVE_MEMALIGN
#undef HAVE_PVALLOC
#else
#define HAVE_MEMALIGN 1
#define HAVE_PVALLOC 1
#endif


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

size_t Tau_page_size(void);

void Tau_detect_memory_leaks(void);

void Tau_track_memory_allocation(void * ptr, size_t size, char const * filename, int lineno);
void Tau_track_memory_deallocation(void * ptr, char const * filename, int lineno);

void * Tau_new(void * ptr, size_t size, char const * filename, int lineno);

void * Tau_malloc(size_t size, char const * filename, int lineno);
void * Tau_calloc(size_t elemCount, size_t elemSize, char const * filename, int lineno);
void   Tau_free(void * baseAdr, char const * filename, int lineno);
#ifdef HAVE_MEMALIGN
void * Tau_memalign(size_t alignment, size_t userSize, char const * filename, int lineno);
#endif
int    Tau_posix_memalign(void **memptr, size_t alignment, size_t userSize, char const * filename, int lineno);
void * Tau_realloc(void * baseAdr, size_t newSize, char const * filename, int lineno);
void * Tau_valloc(size_t size, char const * filename, int lineno);
#ifdef HAVE_PVALLOC
void * Tau_pvalloc(size_t size, char const * filename, int lineno);
#endif

char * Tau_strdup(char const * str, char const * filename, int lineno);
void * Tau_memcpy(void * dest, void const * src, size_t size, char const * filename, int lineno);
char * Tau_strcpy(char * dest, char const * src, char const * filename, int lineno);
char * Tau_strncpy(char * dest, char const * src, size_t size, char const * filename, int lineno);
char * Tau_strcat(char * dest, char const * src, char const * filename, int lineno);
char * Tau_strncat(char * dest, char const * src, size_t size, char const * filename, int lineno);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TAU_MEMORY_H_ */

/***************************************************************************
 * $RCSfile: TauMemory.h,v $   $Author: amorris $
 * $Revision: 1.4 $   $Date: 2010/02/03 06:09:44 $
 * TAU_VERSION_ID: $Id: TauMemory.h,v 1.4 2010/02/03 06:09:44 amorris Exp $ 
 ***************************************************************************/
