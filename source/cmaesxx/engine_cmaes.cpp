#include <stdexcept>
#include <chrono>
#include "engine_cmaes.hpp"
#include "engine_cmaes_utils.hpp"



CmaesEngine::CmaesEngine(int dim, double (*fun) (double*, int), int restart) : dim_(dim)
{
		gt0_ = std::chrono::system_clock::now();
		CmaesEngine::fitfun_ = fun;
        arFunvals_ = cmaes_init(&evo_, dim, NULL, NULL, 0,	"./cmaes_initials.par");
		printf("%s\n", cmaes_SayHello(&evo_));


		_lambda = 128;
		_restart = false;
		_step = 0;
	    _elapsedTime = 0.0;
}

CmaesEngine::~CmaesEngine(){

}

cmaes_t* CmaesEngine::getEvo() {
	return &evo_;
}

double* CmaesEngine::getBestEver() {
	return evo_.rgxbestever;
}


double CmaesEngine::evaluate_population( cmaes_t *evo, double *arFunvals, int step ) {

    int info[4];
    auto tt0 = std::chrono::system_clock::now();
    	
    for( int i = 0; i < _lambda; ++i){
        info[0] = 0; info[1] = 0; info[2] = step; info[3] = i;     /* gen, chain, step, task */
        CmaesEngine::taskfun_(pop_[i], &dim_, &arFunvals_[i]);

    }

    // subtract the log-prior from the log-likelohood
    for( int i=0; i<_lambda; i++)
        arFunvals_[i] -= kb->getTotalDensityLog(pop_[i]);

    auto tt1 = std::chrono::system_clock::now();
  
    return std::chrono::duration<double>(tt1-tt0).count();
};


double CmaesEngine::run() {

	gt1_ = std::chrono::system_clock::now();

	double dt;

	while( !cmaes_TestForTermination(&evo_) ){

        pop_ = cmaes_SamplePopulation(&evo_); 
		cmaes_utils_make_all_points_feasible( &evo_, pop_);
            dt = evaluate_population( &evo_, arFunvals_, _step);
        _elapsedTime += dt;
	
        cmaes_UpdateDistribution(1, &evo_, arFunvals_);

        
        _step++;
    }

    gt2_ = std::chrono::system_clock::now();

    printf("Stop:\n %s \n",  cmaes_TestForTermination(&evo_)); /* print termination reason */
		cmaes_PrintResults(&evo_);

    gt3_ = std::chrono::system_clock::now();
    
    printf("Total elapsed time      = %.3lf  seconds\n", std::chrono::duration<double>(gt3_-gt0_).count());
    printf("Initialization time     = %.3lf  seconds\n", std::chrono::duration<double>(gt1_-gt0_).count());
    printf("Processing time         = %.3lf  seconds\n", std::chrono::duration<double>(gt2_-gt1_).count());
    printf("Funtion Evaluation time = %.3lf  seconds\n", _elapsedTime);
    printf("Finalization time       = %.3lf  seconds\n", std::chrono::duration<double>(gt3_-gt2_).count());

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

void CmaesEngine::cmaes_utils_make_all_points_feasible( cmaes_t *evo, double* const *pop )
{

	int lambda = kb->_lambda;
    int dim    = kb->_dimCount;

	for( int i=0; i<lambda; ++i)
    	while( !is_feasible( pop[i], dim ) )  cmaes_ReSampleSingle( evo, i );

}

double (*CmaesEngine::fitfun_) (double*, int);

void CmaesEngine::taskfun_(double *x, int *n, double *res)
{
    (*res) = - CmaesEngine::fitfun_(x, *n);    // minus for minimization
	return;
}
