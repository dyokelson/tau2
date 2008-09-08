/*
 * This file is primarily lifted from VampirTrace
*/

/**
 * VampirTrace
 * http://www.tu-dresden.de/zih/vampirtrace
 *
 * Copyright (c) 2005-2008, ZIH, TU Dresden, Federal Republic of Germany
 *
 * Copyright (c) 1998-2005, Forschungszentrum Juelich GmbH, Federal
 * Republic of Germany
 *
 * See the file COPYRIGHT in the package base directory for details
 **/

#include <TAU.h>

#if HAVE_CONFIG_H
#  include <config.h>
#endif


#ifdef TAU_BFD
#  include "bfd.h"
#  if defined(HAVE_GNU_DEMANGLE) && HAVE_GNU_DEMANGLE
#    include "demangle.h"
#  endif /* HAVE_GNU_DEMANGLE */
#endif /* TAU_BFD */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#if (defined (VT_OMPI) || defined (VT_OMP))
#  include <omp.h>
#endif

static int gnu_init = 1;       /* is initialization needed? */

/*
 *-----------------------------------------------------------------------------
 * Simple hash table to map function addresses to region names/identifier
 *-----------------------------------------------------------------------------
 */

typedef struct HN {
  long id;            /* hash code (address of function */
  const char* name;   /* associated function name       */
  const char* fname;  /*            file name           */
  int lno;            /*            line number         */
  int vtid;      /* associated region identifier   */
  FunctionInfo *fi;
  struct HN* next;
} HashNode;

#define HASH_MAX 1021

static HashNode* htab[HASH_MAX];

/*
 * Stores function name `n' under hash code `h'
 */

#define VT_NO_ID -1
#define VT_NO_LNO 0

static void hash_put(long h, const char* n, const char* fn, int lno) {
  long id = h % HASH_MAX;
  HashNode *add = (HashNode*)malloc(sizeof(HashNode));
  add->id = h;
  add->name  = n;
  add->fname = fn ? (const char*)strdup(fn) : fn;
  add->lno   = lno;
  add->vtid = VT_NO_ID;
  add->fi = NULL;
  add->next = htab[id];
  htab[id] = add;
}

/*
 * Lookup hash code `h'
 * Returns hash table entry if already stored, otherwise NULL
 */

static HashNode* hash_get(long h) {
  long id = h % HASH_MAX;
  HashNode *curr = htab[id];
  while ( curr ) {
    if ( curr->id == h ) {
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}

#ifdef TAU_BFD

/*
 * Get symbol table by using BFD
 */

static void get_symtab_bfd(void) {
   bfd * BfdImage = 0;
   int nr_all_syms;
   int i; 
   size_t size;
   char* exe_env;
   asymbol **syms;
   int do_getsrc = 1;
#if defined(HAVE_GNU_DEMANGLE) && HAVE_GNU_DEMANGLE
   int do_demangle = 1;
#endif /* HAVE_GNU_DEMANGLE */

   /* initialize BFD */
   bfd_init();

   /* get executable path from environment var. VT_APPPATH */
   /* get executable image */
   BfdImage = bfd_openr("/proc/self/exe", 0 );
   if ( ! BfdImage )
     fprintf (stderr,"BFD: bfd_openr(): failed\n");
   

   /* check image format */
   if ( ! bfd_check_format(BfdImage, bfd_object) ) { 
     printf("BFD: bfd_check_format(): failed");
   }
   
   /* return if file has no symbols at all */
   if ( ! ( bfd_get_file_flags(BfdImage) & HAS_SYMS ) )
     printf("BFD: bfd_get_file_flags(): failed");
   
   /* get the upper bound number of symbols */
   size = bfd_get_symtab_upper_bound(BfdImage);
   
   /* HAS_SYMS can be set even with no symbols in the file! */
   if ( size < 1 )
     printf("BFD: bfd_get_symtab_upper_bound(): < 1");
   
   /* read canonicalized symbols */
   syms = (asymbol **)malloc(size);
   nr_all_syms = bfd_canonicalize_symtab(BfdImage, syms);
   if ( nr_all_syms < 1 )
     printf("BFD: bfd_canonicalize_symtab(): < 1");
   
   for (i=0; i<nr_all_syms; ++i) {
      char* dem_name = 0;
      long addr;
      const char* filename;
      const char* funcname;
      unsigned int lno;
      
      /* ignore system functions */
      if ( strncmp(syms[i]->name, "__", 2) == 0 ||
	   strncmp(syms[i]->name, "bfd_", 4) == 0 ||
	   strstr(syms[i]->name, "@@") != NULL ) continue;

      /* get filename and linenumber from debug info */
      /* needs -g */
      filename = NULL;
      lno = VT_NO_LNO;
      if ( do_getsrc ) {
	bfd_find_nearest_line(BfdImage, bfd_get_section(syms[i]), syms,
			      syms[i]->value, &filename, &funcname, &lno);
      }

      /* calculate function address */
      addr = syms[i]->section->vma+syms[i]->value;

      /* use demangled name if possible */
#if defined(HAVE_GNU_DEMANGLE) && HAVE_GNU_DEMANGLE
      if ( do_demangle ) {
	dem_name = cplus_demangle(syms[i]->name,
				  DMGL_PARAMS | DMGL_ANSI 
				  | DMGL_VERBOSE | DMGL_TYPES);
      }
#endif /* HAVE_GNU_DEMANGLE */

      if( dem_name ) {
	hash_put(addr, dem_name, filename, lno);
      } else {
	char *n = strdup(syms[i]->name);
	hash_put(addr, n, filename, lno);
      }
   }

   free(syms);
   bfd_close(BfdImage);
   return;
}
#endif

/*
 * Get symbol table either by using BFD or by parsing nm-file
 */
static void get_symtab(void)
{
#ifdef TAU_BFD
    get_symtab_bfd();
#else
    printf("No symbol list file given. Please set the environment variable VT_NMFILE to the path of your symbol list file, created with 'nm'.");
#endif

}

/*
 * Register new region
 */

static void register_region(HashNode *hn) {
  int fid = VT_NO_ID;
  int lno = VT_NO_LNO;

  static int counter=0;

  /* -- register file if available -- */
/*   if (hn->fname != NULL) */
/*   { */
/*     fid = vt_def_file(hn->fname); */
/*     lno = hn->lno; */
/*   } */

  /* -- register region and store region identifier -- */
  hn->vtid = counter++;
    /*vt_def_region(hn->name, fid, lno, VT_NO_LNO, VT_DEF_GROUP, VT_FUNCTION); */
}

void gnu_finalize(void);
extern "C" void __cyg_profile_func_enter(void* func, void* callsite);
extern "C" void __cyg_profile_func_exit(void* func, void* callsite);

/*
 * Finalize instrumentation interface
 */

void gnu_finalize() {
  int i;

  for ( i = 0; i < HASH_MAX; i++ )
  {
    if ( htab[i] ) {
      free(htab[i]);
      htab[i] = NULL;
    }
  }
  gnu_init = 1;
}

/*
 * This function is called at the entry of each function
 * The call is generated by the GNU/Intel (>=v10) compilers
 */


extern "C" void atexit_handler() {
  TAU_PROFILE_EXIT("");
}

extern "C" void __cyg_profile_func_enter(void* func, void* callsite) {
  HashNode *hn;

  void * funcptr = func;


#ifdef __ia64__
  funcptr = *( void ** )func;
#endif

  /* -- if not yet initialized, initialize VampirTrace -- */
  if ( gnu_init ) {
    gnu_init = 0;
    get_symtab();
    //    atexit(atexit_handler);
    TheUsingCompInst() = 1;
    // initialization
  }


  /* -- get region identifier -- */
  if ( (hn = hash_get((long)funcptr))) {
    if ( hn->vtid == VT_NO_ID ) {
      /* -- region entered the first time, register region -- */
#     if defined (VT_OMPI) || defined (VT_OMP)
      if (omp_in_parallel()) {
#       pragma omp critical (vt_comp_gnu_1)
	{
	  if ( hn->vtid == VT_NO_ID ) {
	    register_region(hn);
	  }
	}
      } else {
	register_region(hn);
      }
#     else
      register_region(hn);
#     endif
    }

    if ( hn->fi == NULL) {

      // remove the path
      const char *filename = hn->fname;
      while (strchr(filename,'/') != NULL) {
	filename = strchr(filename,'/')+1;
      }


      char routine[2048];

      sprintf (routine, "%s [{%s} {%d,0}]", hn->name, filename, hn->lno);
      void *handle=NULL;
      TAU_PROFILER_CREATE(handle, routine, "", TAU_DEFAULT);
      hn->fi = (FunctionInfo*) handle;
    } 

    Tau_start_timer(hn->fi,0);
    //TAU_START(hn->name);

    //    printf ("name = %s : ", hn->name);
  } else {

    printf ("NOT FOUND! : \n");
  }

  //  printf ("enter, func = %p, callsite = %p\n", func, callsite);

}

/*
 * This function is called at the exit of each function
 * The call is generated by the GNU/Intel (>=v10) compilers
 */

extern "C" void __cyg_profile_func_exit(void* func, void* callsite) {
  HashNode *hn;
  void * funcptr = func;

  //  TAU_GLOBAL_TIMER_STOP();

#ifdef __ia64__
  funcptr = *( void ** )func;
#endif

  if ( (hn = hash_get((long)funcptr)) ) {
    //printf ("name = %s : ", hn->name);
    Tau_stop_timer(hn->fi);
    //TAU_STOP(hn->name);
  } else {
    printf ("NOT FOUND! : ");
  }

  //printf ("exit, func = %p, callsite = %p\n", func, callsite);

}
