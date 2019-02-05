#include "base.h"
#include <chrono>

Korali::KoraliBase* __kbRuntime;

Korali::KoraliBase::KoraliBase(size_t dim, double (*fun) (double*, int), size_t seed = 0)
{
	_seed = seed;
	_dimCount = dim;
	_lambda = -1;
	_rankId = -1;
	_generation = 0;

	gsl_rng_env_setup();
	_dims = new Dimension[dim];
	for (int i = 0; i < dim; i++) _dims[i].setSeed(seed++);
	_gaussianGenerator = new GaussianDistribution(0, 1, _seed++);
	_fitnessFunction = fun;

	_maxFitnessEvaluations = 900*(_dimCount+3)*(_dimCount+3);
	_maxGenerations = std::numeric_limits<size_t>::max();
}


double Korali::KoraliBase::getTotalDensity(double* x)
{
 double density = 1.0;
 for (int i = 0; i < _dimCount; i++) density *= _dims[i].getPriorDistribution()->getDensity(x[i]);
 return density;
}

double Korali::KoraliBase::getTotalDensityLog(double* x)
{
 double densityLog = 0.0;
 for (int i = 0; i < _dimCount; i++) densityLog += _dims[i].getPriorDistribution()->getDensityLog(x[i]);
 return densityLog;
}

void Korali::KoraliBase::run()
{
	upcxx::init();
	_rankId = upcxx::rank_me();
	__kbRuntime = this;

  // Checking Lambda's value
  if(_lambda < 1 )  { fprintf( stderr, "[Korali] Error: Lambda (%lu) should be higher than one.\n", _lambda); exit(-1); }

  //	Use broadcast to send the sample vector to everyone.
  //	Take sample vector away from CMAES and put it into base
  // Allocating sample matrix
  _samplePopulation = (double *) calloc (sizeof(double), _lambda*_dimCount);

	// Initializing solver variables
  _continueEvaluations = true;

  if (_rankId != 0) while(__kbRuntime->_continueEvaluations) upcxx::progress();
  else
  {
    auto startTime = std::chrono::system_clock::now();
  	for (int i = 1; i < upcxx::rank_n(); i++) _workers.push(i);
  	Korali_InitializeInternalVariables();
  	_fitnessVector = (double*) calloc (sizeof(double), _lambda);

		while( !Korali_CheckTermination() )
		{
	  	  upcxx::future<> fut_all = upcxx::make_future();
				Korali_GetSamplePopulation();

				for(int i = 0; i < _lambda; i++) //_fitnessVector[i] = -_fitnessFunction(&_samplePopulation[i*_dimCount], _dimCount);
				{
					while(_workers.empty()) upcxx::progress();
          auto fut = upcxx::rpc(_workers.front(), workerEvaluateFitnessFunction, i, _samplePopulation[i*_dimCount + 0], _samplePopulation[i*_dimCount + 1], _samplePopulation[i*_dimCount + 2], _samplePopulation[i*_dimCount + 3]);
          fut_all = upcxx::when_all(fut_all, fut);
          _workers.pop();
				}

				fut_all.wait();

				for(int i = 0; i < _lambda; i++) _fitnessVector[i] -= getTotalDensityLog(&_samplePopulation[i*_dimCount]);
				Korali_UpdateDistribution(_fitnessVector);
				_generation++;
		}

		auto endTime = std::chrono::system_clock::now();

		if (_rankId == 0) Korali_PrintResults(); // Printing Solver results
		if (_rankId == 0) printf("Total elapsed time      = %.3lf  seconds\n", std::chrono::duration<double>(endTime-startTime).count());

		if (_rankId == 0)	for (int i = 1; i < upcxx::rank_n(); i++) upcxx::rpc_ff(i, finalizeEvaluation);
	}

	upcxx::barrier();
  upcxx::finalize();
}

void Korali::workerComeback(size_t worker, size_t position, double fitness)
{
	__kbRuntime->_fitnessVector[position] = fitness;
	__kbRuntime->_workers.push(worker);
}

void Korali::workerEvaluateFitnessFunction(size_t position, double d0, double d1, double d2, double d3)
{
	//printf("I[workerEvaluateFitnessFunction] rank: %lu\n", __kbRuntime->_rankId);

	double sample[4];
	sample[0] = d0;
	sample[1] = d1;
	sample[2] = d2;
	sample[3] = d3;
	double fitness = -__kbRuntime->_fitnessFunction(sample, __kbRuntime->_dimCount);

	upcxx::rpc_ff(0, workerComeback, __kbRuntime->_rankId, position, fitness);
}

void Korali::finalizeEvaluation() { __kbRuntime->_continueEvaluations = false; }
