#include "problem.h"
#include <time.h>
#include <stdlib.h>

Korali::Problem::Problem(double (*fun) (double*, int, void*), size_t seed)
{
	_seed = seed;
	if (_seed == 0) _seed = clock();
	_fitnessFunction = fun;
	_dimCount = 0;
	_refDataBuffer = 0;
  gsl_rng_env_setup();
}

void Korali::Problem::setReferenceData(void* refData)
{
	_refDataBuffer = refData;
}

void Korali::Problem::addParameter(Parameter p)
{
	_parameters.push_back(p);
	p.setSeed(_seed++);
	_dimCount = _parameters.size();
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
  return _fitnessFunction(sample, _dimCount, _refDataBuffer);
}


