/* --------------------------------------------------------- */
/* --- File: cmaes_interface.h - Author: Nikolaus Hansen --- */
/* ---------------------- last modified:  IV 2007        --- */
/* --------------------------------- by: Nikolaus Hansen --- */
/* --------------------------------------------------------- */
/*   
     CMA-ES for non-linear function minimization. 

     Copyright (C) 1996, 2003, 2007 Nikolaus Hansen. 
     e-mail: hansen AT lri.fr
     
     Documentation: see file docfunctions.txt
     
     License: see file cmaes.c
*/

#ifndef CMAES_H
#define CMAES_H

#include "korali.h"

/* --------------------------------------------------------- */
/* ------------------ Interface ---------------------------- */
/* --------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* --- initialization, constructors, destructors --- */
double * cmaes_init();

void cmaes_exit();
void cmaes_PrintResults();
/* --- core functions --- */
double * const * cmaes_SamplePopulation();
double *         cmaes_UpdateDistribution(int save_hsit,
                 const double *rgFitnessValues);
bool     cmaes_TestForTermination();

/* --- additional functions --- */
double * const * cmaes_ReSampleSingle( int index);
double *         cmaes_SampleSingleInto( double *rgx);
void             cmaes_UpdateEigensystem( int flgforce);

/* --- getter functions --- */
double         cmaes_Get( char const *keyword);
const double * cmaes_GetPtr( char const *keyword); /* e.g. "xbestever" */
double *       cmaes_GetNew( char const *keyword); /* user is responsible to free */
double *       cmaes_GetInto( char const *keyword, double *mem); /* allocs if mem==NULL, user is responsible to free */

/* --- online control and output --- */
void           cmaes_ReadSignals( char const *filename);
void           cmaes_WriteToFile( const char *szKeyWord,
                                 const char *output_filename); 
char *         cmaes_SayHello();
/* --- misc --- */
double *       cmaes_NewDouble(int n); /* user is responsible to free */
void           cmaes_FATAL(char const *s1, char const *s2, char const *s3, 
               char const *s4);

    
#ifdef __cplusplus
} // end extern "C"
#endif

#endif // CMAES_H
