#include "problem.h"
#include <time.h>

Korali::Problem::Problem(std::string type, double (*fun) (double*, int), size_t seed)
{
	_seed = seed;

	if (_seed == 0) _seed = clock();

	_type = type;
	_dimCount = 0;

  gsl_rng_env_setup();

	_fitnessFunction = fun;
}

void Korali::Problem::addParameter(Parameter p)
{
	_parameters.push_back(p);
	p.setSeed(_seed++);
	_dimCount++;
}

double Korali::Problem::getTotalDensity(double* x)
{
 double density = 1.0;
 for (int i = 0; i < _parameters.size(); i++) density *= _parameters[i].getPriorDistribution()->getDensity(x[i]);
 return density;
}

double Korali::Problem::getTotalDensityLog(double* x)
{
 double densityLog = 0.0;
 for (int i = 0; i < _dimCount; i++) densityLog += _parameters[i].getPriorDistribution()->getDensityLog(x[i]);
 return densityLog;
}

double Korali::Problem::evaluateFitness(double* sample)
{
	if (_type == "Minimizer") { return  _fitnessFunction(sample, _dimCount); }
	if (_type == "Maximizer") { return  -_fitnessFunction(sample, _dimCount); }

	return -1.0;
}

