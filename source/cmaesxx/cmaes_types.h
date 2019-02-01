/* --------------------------------------------------------- */
/* --- File: cmaes.h ----------- Author: Nikolaus Hansen --- */
/* ---------------------- last modified: IX 2010         --- */
/* --------------------------------- by: Nikolaus Hansen --- */
/* --------------------------------------------------------- */
/*   
     CMA-ES for non-linear function minimization. 

     Copyright (C) 1996, 2003-2010  Nikolaus Hansen. 
     e-mail: nikolaus.hansen (you know what) inria.fr
      
     License: see file cmaes.c
   
*/
#ifndef CMAES_TYPES_H
#define CMAES_TYPES_H

#include <time.h>
#include "korali.h"


typedef struct 
{


  double sigma;  /* step size */

  double *rgxmean;  /* mean x vector, "parent" */
  double *rgxbestever; 
  double **rgrgx;   /* range of x-vectors, lambda offspring */
  int *index;       /* sorting index of sample pop. */
  double *arFuncValueHist;

  short flgIniphase; /* not really in use anymore */
  short flgStop; 

  double chiN; 
  double **C;  /* lower triangular matrix: i>=j for C[i][j] */
  double **B;  /* matrix with normalize eigenvectors in columns */
  double *rgD; /* axis lengths */

  double *rgpc;
  double *rgps;
  double *rgxold; 
  double *rgout; 
  double *rgBDz;   /* for B*D*z */
  double *rgdTmp;  /* temporary (random) vector used in different places */
  double *rgFuncValue; 
  double *publicFitness; /* returned by cmaes_init() */

  double gen; /* Generation number */
  double countevals;
  double state; /* 1 == sampled, 2 == not in use anymore, 3 == updated */

  double maxdiagC; /* repeatedly used for output */
  double mindiagC;
  double maxEW;
  double minEW;

  char sOutString[330]; /* 4x80 */

  short flgEigensysIsUptodate;
  short flgCheckEigen; /* control via cmaes_signals.par */
  double genOfEigensysUpdate; 


  double dMaxSignifKond;
  double dLastMinEWgroesserNull;


} cmaes_t; 



/*
  storage for distribution parameters:
  mean mu
  Covariant matrix C decomposed as:
  - C = Q D Q^{-1} if !flgdiag
  - C = D          if  flgdiag
  only diagonal elements are stored in D 
 */

typedef struct {
    int dim, flgdiag;
    double **Q;
    double *D;
    double *mu;
    double *w; // workspace
} cmaes_distr_t;

#endif // CMAES_TYPES_H
