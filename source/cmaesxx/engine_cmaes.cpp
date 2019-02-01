#include <stdexcept>
#include <chrono>
#include "engine_cmaes.hpp"
#include "engine_cmaes_utils.hpp"



CmaesEngine::CmaesEngine(int dim, double (*fun) (double*, int), int restart) : dim_(dim)
{
		fitfun_ = fun;
    arFunvals_ = cmaes_init();

    printf("(%d,%d)-CMA-ES(mu_eff=%.1f), dimension=%d, diagonalIterations=%ld, randomSeed=%d",
            kb->_mu, kb->_lambda, kb->_muEffective,  kb->_dimCount, kb->_diagonalCovarianceMatrixEvalFrequency,
            kb->_seed);
}

double CmaesEngine::evaluate_population( double *arFunvals) {

    auto tt0 = std::chrono::system_clock::now();
    for( int i = 0; i < kb->_lambda; ++i) arFunvals_[i] = - fitfun_(pop_[i], dim_);
    for( int i = 0; i < kb->_lambda; i++) arFunvals_[i] -= kb->getTotalDensityLog(pop_[i]);
    auto tt1 = std::chrono::system_clock::now();
    return std::chrono::duration<double>(tt1-tt0).count();
};


double CmaesEngine::run() {

	auto startTime = std::chrono::system_clock::now();


	while( !cmaes_TestForTermination() )
	{
        pop_ = cmaes_SamplePopulation();
        for(int i = 0; i < kb->_lambda; ++i)	while( !is_feasible( pop_[i], kb->_dimCount )) cmaes_ReSampleSingle(i );
        for(int i = 0; i < kb->_lambda; ++i) arFunvals_[i] = - fitfun_(pop_[i], dim_);
        for(int i = 0; i < kb->_lambda; i++) arFunvals_[i] -= kb->getTotalDensityLog(pop_[i]);
        cmaes_UpdateDistribution(1, arFunvals_);
  }

	auto endTime = std::chrono::system_clock::now();

		cmaes_PrintResults();

    
    printf("Total elapsed time      = %.3lf  seconds\n", std::chrono::duration<double>(endTime-startTime).count());

	return 0.0;
}


int CmaesEngine::is_feasible(double *pop, int dim) {
    int i, good;
    for (i = 0; i < dim; i++) {
        good = (kb->_dims[i]._lowerBound <= pop[i]) && (pop[i] <= kb->_dims[i]._upperBound);
        if (!good) {
            return 0;
        }
    }
    return 1;
}

