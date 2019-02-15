#include "problem.h"

Korali::Problem::Problem(std::string type, size_t dim, double (*fun) (double*, int), size_t seed)
{
	_seed = seed;
	_type = type;
	_priors = new Prior[dim];
	_dimCount = dim;

  gsl_rng_env_setup();
	for (int i = 0; i < dim; i++) _priors[i].setSeed(_seed++);

	_fitnessFunction = fun;
}

double Korali::Problem::getTotalDensity(double* x)
{
 double density = 1.0;
 for (int i = 0; i < _dimCount; i++) density *= _priors[i].getPriorDistribution()->getDensity(x[i]);
 return density;
}

double Korali::Problem::getTotalDensityLog(double* x)
{
 double densityLog = 0.0;
 for (int i = 0; i < _dimCount; i++) densityLog += _priors[i].getPriorDistribution()->getDensityLog(x[i]);
 return densityLog;
}

double Korali::Problem::evaluateFitness(double* sample)
{
	if (_type == "Minimizer") { return  _fitnessFunction(sample, _dimCount); }
	if (_type == "Maximizer") { return  -_fitnessFunction(sample, _dimCount); }

	return -1.0;
}

