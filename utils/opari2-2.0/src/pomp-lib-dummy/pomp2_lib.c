/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */
/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/
/** @internal
 *
 *  @file       pomp2_lib.c
 *  @status     alpha
 *
 *  @maintainer Dirk Schmidl <schmidl@rz.rwth-aachen.de>
 *
 *  @brief      Dummy implementation of all POMP2 Functions. These functions
 *              only print out messages and do not measure anything.*/

#include <config.h>
#include "pomp2_lib.h"

/* *INDENT-OFF*  */
/** todo include again if c part is ready */
/* #include "pomp2_fwrapper_def.h" */

#include "pomp2_region_info.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @name Functions generated by the instrumenter */
/*@{*/
/**
 * Returns the number of instrumented regions.@n
 * The instrumenter scans all opari-created include files with nm and greps
 * the POMP2_INIT_uuid_numRegions() function calls. Here we return the sum of
 * all numRegions.
 */
extern size_t
POMP2_Get_num_regions();

/**
 * Init all opari-created regions.@n
 * The instrumentor scans all opari-created include files with nm and greps
 * the POMP2_INIT_uuid_numRegions() function calls. The instrumentor then
 * defines this functions by calling all grepped functions.
 */
extern void
POMP2_Init_regions();

/**
 * Returns the opari version.
 */
extern const char*
POMP2_Get_opari2_version();

/*@}*/

/** @brief All relevant information for a region is stored here */
typedef struct
{
    /** region type of construct */
    char*  rtype;
    /** critical or user region name */
    char*  name;
    /** sections only: number of sections */
    int    num_sections;

    /** start file name*/
    char*  start_file_name;
    /** line number 1*/
    int    start_line_1;
    /** line number 2*/
    int    start_line_2;

    /** end file name*/
    char*  end_file_name;
    /** line number 1*/
    int    end_line_1;
    /** line number 2*/
    int    end_line_2;
    /** region id*/
    size_t id;
} my_pomp2_region;

static void
free_my_pomp2_region_member( char** member )
{
    if ( *member )
    {
        free( *member );
        *member = 0;
    }
}

static void
free_my_pomp2_region_members( my_pomp2_region* region )
{
    if ( region )
    {
        free_my_pomp2_region_member( &region->rtype );
        free_my_pomp2_region_member( &region->name );
        free_my_pomp2_region_member( &region->start_file_name );
        free_my_pomp2_region_member( &region->end_file_name );
    }
}

static void
assignString( char**      destination,
              const char* source )
{
    assert( source );
    *destination = malloc( strlen( source ) * sizeof( char ) + 1 );
    strcpy( *destination, source );
}


static void
initDummyRegionFromPOMP2RegionInfo(
    my_pomp2_region*         pomp2_region,
    const POMP2_Region_info* pomp2RegionInfo )
{
    assignString( &( pomp2_region->rtype ),
                  pomp2RegionType2String( pomp2RegionInfo->mRegionType ) );

    assignString( &pomp2_region->start_file_name,
                  pomp2RegionInfo->mStartFileName );
    pomp2_region->start_line_1 = pomp2RegionInfo->mStartLine1;
    pomp2_region->start_line_2 = pomp2RegionInfo->mStartLine2;

    assignString( &pomp2_region->end_file_name,
                  pomp2RegionInfo->mEndFileName );
    pomp2_region->end_line_1 = pomp2RegionInfo->mEndLine1;
    pomp2_region->end_line_2 = pomp2RegionInfo->mEndLine2;

    if ( pomp2RegionInfo->mRegionType == POMP2_User_region )
    {
        assignString( &pomp2_region->name,
                      pomp2RegionInfo->mUserRegionName );
    }
    else if ( pomp2RegionInfo->mRegionType == POMP2_Critical && pomp2RegionInfo->mCriticalName )
    {
        assignString( &pomp2_region->name,
                      pomp2RegionInfo->mCriticalName );
    }

    pomp2_region->num_sections = pomp2RegionInfo->mNumSections;
}


/*
 * Global variables
 */

int              pomp2_tracing = 0;
my_pomp2_region* my_pomp2_regions;

/*
 * C pomp2 function library
 */



void
POMP2_Finalize()
{
    static int   pomp2_finalize_called = 0;
    size_t       i;
    const size_t nRegions = POMP2_Get_num_regions();

    if ( my_pomp2_regions )
    {
        for ( i = 0; i < nRegions; ++i )
        {
            free_my_pomp2_region_members( &my_pomp2_regions[ i ] );
        }
        free( my_pomp2_regions );
        my_pomp2_regions = 0;
    }

    if ( !pomp2_finalize_called )
    {
        pomp2_finalize_called = 1;
        fprintf( stderr, "  0: finalize\n" );
    }
}

void
POMP2_Init()
{
    static int pomp2_init_called = 0;

    if ( !pomp2_init_called )
    {
        pomp2_init_called = 1;

        atexit( POMP2_Finalize );
        fprintf( stderr, "  0: init\n" );

        /* Allocate memory for your POMP2_Get_num_regions() regions */
        my_pomp2_regions = calloc( POMP2_Get_num_regions(),
                                   sizeof( my_pomp2_region ) );
        //pomp2_tpd_ = ( void* )malloc( sizeof( int ) );
        //pomp2_tpd_ = ( long )0;

        POMP2_Init_regions();

        pomp2_tracing = 1;
    }
}

void
POMP2_Off()
{
    pomp2_tracing = 0;
}

void
POMP2_On()
{
    pomp2_tracing = 1;
}

void
POMP2_Begin( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: begin region %s\n",
                 omp_get_thread_num(), region->name );
    }
}

void
POMP2_End( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: end   region %s\n",
                 omp_get_thread_num(), region->name );
    }
}

void
POMP2_Assign_handle( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
    static size_t count = 0;
    /* printf( "%d POMP2_Assign_handle: \"%s\"\n", (int)count, ctc_string ); */

    POMP2_Region_info pomp2RegionInfo;
    ctcString2RegionInfo( ctc_string, &pomp2RegionInfo );

    initDummyRegionFromPOMP2RegionInfo( &my_pomp2_regions[ count ], &pomp2RegionInfo );
    my_pomp2_regions[ count ].id = count;
    printf( "assign_handle %d %s\n", ( int )count, my_pomp2_regions[ count ].rtype );

    *pomp2_handle = &my_pomp2_regions[ count ];

    freePOMP2RegionInfoMembers( &pomp2RegionInfo );
    ++count;
    assert( count <= POMP2_Get_num_regions() );
}

void
POMP2_Atomic_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: enter atomic\n", omp_get_thread_num() );
    }
}

void
POMP2_Atomic_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: exit  atomic\n", omp_get_thread_num() );
    }
}

void
POMP2_Implicit_barrier_enter( POMP2_Region_handle* pomp2_handle )
{
    POMP2_Barrier_enter( pomp2_handle, "" );
}

extern void
POMP2_Implicit_barrier_exit( POMP2_Region_handle* pomp2_handle )
{
    POMP2_Barrier_exit( pomp2_handle );
}


void
POMP2_Barrier_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        if ( region->rtype[ 0 ] == 'b' )
        {
            fprintf( stderr, "%3d: enter barrier\n", omp_get_thread_num() );
        }
        else
        {
            fprintf( stderr, "%3d: enter implicit barrier of %s\n",
                     omp_get_thread_num(), region->rtype );
        }
    }
}

void
POMP2_Barrier_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        if ( region->rtype[ 0 ] == 'b' )
        {
            fprintf( stderr, "%3d: exit  barrier\n", omp_get_thread_num() );
        }
        else
        {
            fprintf( stderr, "%3d: exit  implicit barrier of %s\n",
                     omp_get_thread_num(), region->rtype );
        }
    }
}

void
POMP2_Flush_enter( POMP2_Region_handle* pomp2_handle,
		   const char           ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: enter flush\n", omp_get_thread_num() );
    }
}

void
POMP2_Flush_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: exit  flush\n", omp_get_thread_num() );
    }
}

void
POMP2_Critical_begin( POMP2_Region_handle* pomp2_handle )
{
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: begin critical %s\n",
                 omp_get_thread_num(), region->rtype );
    }
}

void
POMP2_Critical_end( POMP2_Region_handle* pomp2_handle )
{
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: end   critical %s\n",
                 omp_get_thread_num(), region->name );
    }
}

void
POMP2_Critical_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: enter critical %s\n",
                 omp_get_thread_num(), region->name );
    }
}

void
POMP2_Critical_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    my_pomp2_region* region = *pomp2_handle;
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: exit  critical %s\n",
                 omp_get_thread_num(), region->name );
    }
}

void
POMP2_For_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: enter for\n", omp_get_thread_num() );
    }
}

void
POMP2_For_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: exit  for\n", omp_get_thread_num() );
    }
}

void
POMP2_Master_begin( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: begin master\n", omp_get_thread_num() );
    }
}

void
POMP2_Master_end( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: end   master\n", omp_get_thread_num() );
    }
}

void
POMP2_Parallel_begin( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: begin parallel\n", omp_get_thread_num() );
    }
}

void
POMP2_Parallel_end( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: end   parallel\n", omp_get_thread_num() );
    }
}

void
POMP2_Parallel_fork( POMP2_Region_handle* pomp2_handle,
                     int                  num_threads,
                     const char           ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: fork  parallel\n", omp_get_thread_num() );
    }
}

void
POMP2_Parallel_join( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: join  parallel\n", omp_get_thread_num() );
    }
}

void
POMP2_Section_begin( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: begin section\n", omp_get_thread_num() );
    }
}

void
POMP2_Section_end( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: end   section\n", omp_get_thread_num() );
    }
}

void
POMP2_Sections_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        my_pomp2_region* region = *pomp2_handle;
        fprintf( stderr, "%3d: enter sections (%d)\n",
                 omp_get_thread_num(), region->num_sections );
    }
}

void
POMP2_Sections_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: exit  sections\n", omp_get_thread_num() );
    }
}

void
POMP2_Single_begin( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: begin single\n", omp_get_thread_num() );
    }
}

void
POMP2_Single_end( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: end   single\n", omp_get_thread_num() );
    }
}

void
POMP2_Single_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: enter single\n", omp_get_thread_num() );
    }
}

void
POMP2_Single_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: exit  single\n", omp_get_thread_num() );
    }
}

void
POMP2_Workshare_enter( POMP2_Region_handle* pomp2_handle, const char ctc_string[] )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: enter workshare\n", omp_get_thread_num() );
    }
}

void
POMP2_Workshare_exit( POMP2_Region_handle* pomp2_handle )
{
#pragma omp critical
    if ( *pomp2_handle == NULL )
    {
        POMP2_Init();
    }
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: exit  workshare\n", omp_get_thread_num() );
    }
}

/*
   *----------------------------------------------------------------
 * C Wrapper for OpenMP API
 ******----------------------------------------------------------------
 */

void
POMP2_Init_lock( omp_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: init lock\n", omp_get_thread_num() );
    }
    omp_init_lock( s );
}

void
POMP2_Destroy_lock( omp_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: destroy lock\n", omp_get_thread_num() );
    }
    omp_destroy_lock( s );
}

void
POMP2_Set_lock( omp_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: set lock\n", omp_get_thread_num() );
    }
    omp_set_lock( s );
}

void
POMP2_Unset_lock( omp_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: unset lock\n", omp_get_thread_num() );
    }
    omp_unset_lock( s );
}

int
POMP2_Test_lock( omp_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: test lock\n", omp_get_thread_num() );
    }
    return omp_test_lock( s );
}

void
POMP2_Init_nest_lock( omp_nest_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: init nestlock\n", omp_get_thread_num() );
    }
    omp_init_nest_lock( s );
}

void
POMP2_Destroy_nest_lock( omp_nest_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: destroy nestlock\n", omp_get_thread_num() );
    }
    omp_destroy_nest_lock( s );
}

void
POMP2_Set_nest_lock( omp_nest_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: set nestlock\n", omp_get_thread_num() );
    }
    omp_set_nest_lock( s );
}

void
POMP2_Unset_nest_lock( omp_nest_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: unset nestlock\n", omp_get_thread_num() );
    }
    omp_unset_nest_lock( s );
}

int
POMP2_Test_nest_lock( omp_nest_lock_t* s )
{
    if ( pomp2_tracing )
    {
        fprintf( stderr, "%3d: test nestlock\n", omp_get_thread_num() );
    }
    return omp_test_nest_lock( s );
}

/* *INDENT-OFF*  */
/** @todo include again if c part is ready */
#if 0
/*
   *----------------------------------------------------------------
 * Fortran  Wrapper for OpenMP API
 ******----------------------------------------------------------------
 */
/* *INDENT-OFF*  */
#if defined(__ICC) || defined(__ECC) || defined(_SX)
#define CALLFSUB(a) a
#else
#define CALLFSUB(a) FSUB(a)
#endif

void FSUB(POMP2_Init_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: init lock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_init_lock)(s);
}

void FSUB(POMP2_Destroy_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: destroy lock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_destroy_lock)(s);
}

void FSUB(POMP2_Set_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: set lock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_set_lock)(s);
}

void FSUB(POMP2_Unset_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: unset lock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_unset_lock)(s);
}

int  FSUB(POMP2_Test_lock)(omp_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: test lock\n", omp_get_thread_num());
  }
  return CALLFSUB(omp_test_lock)(s);
}

#ifndef __osf__
void FSUB(POMP2_Init_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: init nestlock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_init_nest_lock)(s);
}

void FSUB(POMP2_Destroy_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: destroy nestlock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_destroy_nest_lock)(s);
}

void FSUB(POMP2_Set_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: set nestlock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_set_nest_lock)(s);
}

void FSUB(POMP2_Unset_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: unset nestlock\n", omp_get_thread_num());
  }
  CALLFSUB(omp_unset_nest_lock)(s);
}

int  FSUB(POMP2_Test_nest_lock)(omp_nest_lock_t *s) {
  if ( pomp2_tracing ) {
    fprintf(stderr, "%3d: test nestlock\n", omp_get_thread_num());
  }
  return CALLFSUB(omp_test_nest_lock)(s);
}
#endif
#endif /*0*/
