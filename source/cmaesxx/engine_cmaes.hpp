/* -------------------------------------------------------------------------- */
/* --- File: engine_cmaes.cpp--- Author: Daniel Waelchli -------------------- */
/* ---------------------- last modified: Jan 2019        -------------------- */
/* --------------------------------- by: Daniel Waelchli -------------------- */
/* -------------------------------------------------------------------------- */
/*
     Cpp Wrapper for parallel CMA-ES for gradient free non-linear
     	function minimization.

	 Implementation based on work of Panagiotis Hadjidoukas (see engine_cmaes.c)
		and Nikolaus Hansen (see cmaes.c)

*/

#ifndef ENGINE_CMAES_HPP
#define ENGINE_CMAES_HPP

#include <string>
#include <stdio.h>
#include <vector>
#include "korali.h"
#include <chrono>
#include <cmaes.h>

class CmaesEngine {

public:
	CmaesEngine(int dim, double (*fun) (double*, int), int restart = 0);

	double run();


private:

	int dim_;

	double *const*pop_;
  double *arFunvals_;

	double (*fitfun_) (double*, int);
  double evaluate_population(double *arFunvals );
  int is_feasible(double *pop, int dim);

};

#endif //ENGINE_CMAES_HPP


