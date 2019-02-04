#include "base.h"
#include <chrono>

Korali::KoraliBase::KoraliBase(size_t dim, double (*fun) (double*, int), size_t seed = 0)
{
	_seed = seed;
	_dimCount = dim;
	_lambda = -1;

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
  // Creating Lambda array
  if(_lambda < 1 )  { fprintf( stderr, "[Korali] Error: Lambda (%lu) should be higher than one.\n", _lambda); exit(-1); }
	_fitnessVector = (double*) calloc (sizeof(double), _lambda);

	// Initializing solver variables
	Korali_InitializeInternalVariables();

	auto startTime = std::chrono::system_clock::now();

	while( !Korali_CheckTermination() )
	{
		_samplePopulation = Korali_GetSamplePopulation();
		for(int i = 0; i < _lambda; ++i) _fitnessVector[i] = - _fitnessFunction(_samplePopulation[i], _dimCount);
		for(int i = 0; i < _lambda; i++) _fitnessVector[i] -= getTotalDensityLog(_samplePopulation[i]);
		Korali_UpdateDistribution(_fitnessVector);
  }

	auto endTime = std::chrono::system_clock::now();

	// Printing Solver results
	Korali_PrintResults();

  printf("Total elapsed time      = %.3lf  seconds\n", std::chrono::duration<double>(endTime-startTime).count());

}
