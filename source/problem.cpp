#include "problem.h"
#include <time.h>
#include <stdlib.h>

Korali::ProblemBase::ProblemBase(void (*fun) (double*, int, double*), size_t seed)
{
	_seed = seed;
	if (_seed == 0) _seed = clock();
	_fitnessFunction = fun;
	_dimCount = 0;
  gsl_rng_env_setup();
}

Korali::Minimizer::Minimizer(  void (*fun) (double*, int, double*), size_t seed) : Korali::ProblemBase::ProblemBase(fun, seed) {}
Korali::Maximizer::Maximizer(  void (*fun) (double*, int, double*), size_t seed) : Korali::ProblemBase::ProblemBase(fun, seed) {}
Korali::Likelihood::Likelihood(void (*fun) (double*, int, double*), size_t seed) : Korali::ProblemBase::ProblemBase(fun, seed) {}

void Korali::ProblemBase::addParameter(Parameter p)
{
	_parameters.push_back(p);
	p.setSeed(_seed++);
	_dimCount = _parameters.size();
}

double Korali::ProblemBase::getTotalDensity(double* x)
{
 double density = 1.0;
 for (int i = 0; i < _parameters.size(); i++) density *= _parameters[i].getPriorDistribution()->getDensity(x[i]);
 return density;
}

double Korali::ProblemBase::getTotalDensityLog(double* x)
{
 double densityLog = 0.0;
 for (int i = 0; i < _dimCount; i++) densityLog += _parameters[i].getPriorDistribution()->getDensityLog(x[i]);
 return densityLog;
}

double Korali::Minimizer::evaluateFitness(double* sample)
{
	double result;
  _fitnessFunction(sample, _dimCount, &result);
  return result;
}

double Korali::Maximizer::evaluateFitness(double* sample)
{
	double result;
  _fitnessFunction(sample, _dimCount, &result);
  return -result;
}

double Korali::Likelihood::evaluateFitness(double* sample)
{
	double* results = (double*) calloc (sizeof(double), _refData.size());
	_fitnessFunction(sample, _dimCount, results);
  return -results[0];
}

void Korali::Likelihood::addReferenceData(double ref)
{
	_refData.push_back(ref);
}
