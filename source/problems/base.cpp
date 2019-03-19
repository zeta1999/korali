#include "problems/base.h"
#include <chrono>

Korali::BaseProblem::BaseProblem(size_t seed)
{
	_seed = seed;
	if (_seed == -1)
	{
    std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::nanoseconds(0));
		_seed  = std::chrono::nanoseconds(now_c).count();
	}
	_parameterCount = 0;
  gsl_rng_env_setup();
}

void Korali::BaseProblem::addParameter(BaseDistribution* p)
{
	if(p->_name == "") p->setName("Parameter" + std::to_string(_parameterCount));
	_parameters.push_back(p);
	_parameterCount = _parameters.size();
}

double Korali::BaseProblem::getPriorsLogProbabilityDensity(double *x)
{
  double logp = 0.0;
  for (int i = 0; i < _parameterCount; i++) logp += _parameters[i]->getDensityLog(x[i]);
  return logp;
}

double Korali::BaseProblem::getPriorsProbabilityDensity(double *x)
{
  double dp = 1.0;
  for (int i = 0; i < _parameterCount; i++) dp *= _parameters[i]->getDensity(x[i]);
  return dp;
}

void Korali::BaseProblem::initializeParameters()
{
  // Initialize Parameter Priors
  for (int i = 0; i < _parameterCount; i++)	_parameters[i]->initializeDistribution(_seed+i);

	// Checking correct parameters for problem
	for (int i = 0; i < _parameterCount; i++) _parameters[i]->checkBounds();
}

