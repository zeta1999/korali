#include <stdexcept>

#include "engine_cmaes.hpp"
#include "engine_cmaes_utils.hpp"

void CmaesEngine::addPrior(Korali::Prior* p)
{
	_priors.push_back(p);
}

void CmaesEngine::addBound(double lower, double upper)
{
	_bounds.push_back( std::pair<double, double>(lower, upper) );
}

CmaesEngine::CmaesEngine(int dim, double (*fun) (double*, int), int restart) : dim_(dim)
{
		gt0_ = get_time();
		CmaesEngine::fitfun_ = fun;
        arFunvals_ = cmaes_init(&evo_, dim, NULL, NULL, 0, 0,	"./cmaes_initials.par");
		printf("%s\n", cmaes_SayHello(&evo_));
		cmaes_ReadSignals(&evo_, "./cmaes_initials.par");

		_lambda = 128;
		_restart = false;
		_step = 0;
	    _elapsedTime = 0.0;
}

CmaesEngine::~CmaesEngine(){
    cmaes_exit(&evo_); /* release memory */
}

cmaes_t* CmaesEngine::getEvo() {
	return &evo_;
}

double* CmaesEngine::getBestEver() {
	return evo_.rgxbestever;
}

double CmaesEngine::getBestFunVal() {
	return cmaes_Get(&evo_,"fbestever");
}

double CmaesEngine::evaluate_population( cmaes_t *evo, double *arFunvals, int step ) {

    int info[4];
    double tt0, tt1 ;
    	
    tt0 = get_time();
	
    for( int i = 0; i < _lambda; ++i){
        info[0] = 0; info[1] = 0; info[2] = step; info[3] = i;     /* gen, chain, step, task */
        CmaesEngine::taskfun_(pop_[i], &dim_, &arFunvals_[i]);

    }

    // subtract the log-prior from the log-likelohood
    for( int i=0; i<_lambda; i++)
      for (int j = 0; j < _priors.size(); j++)
        arFunvals_[i] -= _priors[j]->getDensityLog(pop_[i]);

    tt1 = get_time();
  
    return tt1-tt0;
};


double CmaesEngine::run() {

	gt1_ = get_time();

	double dt;

	while( !cmaes_TestForTermination(&evo_) ){

        pop_ = cmaes_SamplePopulation(&evo_); 
		cmaes_utils_make_all_points_feasible( &evo_, pop_);
            dt = evaluate_population( &evo_, arFunvals_, _step);
        _elapsedTime += dt;
	
        cmaes_UpdateDistribution(1, &evo_, arFunvals_);

        cmaes_ReadSignals(&evo_, "./cmaes_initials.par"); fflush(stdout);

        if (VERBOSE) cmaes_utils_print_the_best(evo_, _step);
		
       	if (_IODUMP_ && !_restart){
            cmaes_utils_write_pop_to_file(evo_, arFunvals_, pop_, _step);
        }

        if( ! cmaes_utils_is_there_enough_time( JOBMAXTIME, gt0_, dt ) ){
            k->_maxGenerations=_step+1;
            break;
        }
        
        _step++;
    }

    gt2_ = get_time();

    printf("Stop:\n %s \n",  cmaes_TestForTermination(&evo_)); /* print termination reason */
    cmaes_WriteToFile( &evo_, "all", "allcmaes.dat" );         /* write final results */

    gt3_ = get_time();
    
    printf("Total elapsed time      = %.3lf  seconds\n", gt3_-gt0_);
    printf("Initialization time     = %.3lf  seconds\n", gt1_-gt0_);
    printf("Processing time         = %.3lf  seconds\n", gt2_-gt1_);
    printf("Funtion Evaluation time = %.3lf  seconds\n", _elapsedTime);
    printf("Finalization time       = %.3lf  seconds\n", gt3_-gt2_);

	return 0.0;
}


int CmaesEngine::is_feasible(double *pop, int dim) {
    int i, good;
    for (i = 0; i < dim; i++) {
        good = (_bounds[i].first <= pop[i]) && (pop[i] <= _bounds[i].second);
        if (!good) {
            return 0;
        }
    }
    return 1;
}

void CmaesEngine::cmaes_utils_make_all_points_feasible( cmaes_t *evo, double* const *pop )
{

	int lambda = cmaes_Get( evo, "lambda");
    int dim    = cmaes_Get( evo, "dim");

	for( int i=0; i<lambda; ++i)
    	while( !is_feasible( pop[i], dim ) )  cmaes_ReSampleSingle( evo, i );

}

double (*CmaesEngine::fitfun_) (double*, int);

void CmaesEngine::taskfun_(double *x, int *n, double *res)
{
    (*res) = - CmaesEngine::fitfun_(x, *n);    // minus for minimization
	return;
}
