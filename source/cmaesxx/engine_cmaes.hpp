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
#include "prior.h"
#include <cmaes.h>
#include <cmaes_utils.h>


#define VERBOSE 0
#define JOBMAXTIME 0
#define _IODUMP_ 1

class CmaesEngine {

public:
	CmaesEngine(int dim, double (*fun) (double*, int), int restart = 0);

	~CmaesEngine();

	double run();

	cmaes_t* getEvo();
	double   getBestFunVal();
	double*  getBestEver();

    void addPrior(Korali::Prior* p);
    void addBound(double lower, double upper);

private:

    double _elapsedTime;
    int _lambda;
    int _step;
    bool _restart;

	cmaes_t evo_;
	
	double gt0_, gt1_, gt2_, gt3_;

	
	int dim_;
	std::vector< std::pair<double, double> > _bounds;
	std::vector<Korali::Prior*> _priors;

	double *const*pop_;
    double *arFunvals_; 

	static double (*fitfun_) (double*, int);
	static void taskfun_(double *x, int *no, double* res);
    double evaluate_population( cmaes_t *evo, double *arFunvals, int step );
    void   cmaes_utils_make_all_points_feasible( cmaes_t *evo, double * const *pop );
    int is_feasible(double *pop, int dim);


};

#endif //ENGINE_CMAES_HPP


