#include "problems/problem.h"
#include <stdlib.h>
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

void Korali::BaseProblem::addParameter(Parameter p)
{
	if(p._name == "") p._name = "Parameter" + std::to_string(_parameterCount);
	_parameters.push_back(p);
	_parameterCount = _parameters.size();
}

double Korali::BaseProblem::getPriorsLogProbabilityDensity(double *x)
{
  double logp = 0.0;
  for (int i = 0; i < _parameterCount; i++) logp += _parameters[i].getDensityLog(x[i]);
  return logp;
}

double Korali::BaseProblem::getPriorsProbabilityDensity(double *x)
{
  double dp = 1.0;
  for (int i = 0; i < _parameterCount; i++) dp *= _parameters[i].getDensity(x[i]);
  return dp;
}

void Korali::BaseProblem::initializeParameters()
{
  // Initialize Parameter Priors
  for (int i = 0; i < _parameterCount; i++)	_parameters[i].initialize(_seed+i+1);

	// Checking correct parameters for problem
	for (int i = 0; i < _parameterCount; i++) _parameters[i].checkBounds();
}

Korali::DirectEvaluation::DirectEvaluation(double (*modelFunction) (double*), size_t seed) : Korali::BaseProblem::BaseProblem(seed)
{
	_modelFunction = modelFunction;
}

double Korali::DirectEvaluation::evaluateFitness(double* sample)
{
  return _modelFunction(sample);
}

Korali::Likelihood::Likelihood(double* (*modelFunction) (double*), size_t seed) : Korali::BaseProblem::BaseProblem(seed)
{
	_referenceData = NULL;
	_nData = 0;
	_modelFunction = modelFunction;
	_referenceDataSet = false;

	Korali::Parameter sigma("Sigma");
	sigma.setInitialX(10.0);
	sigma.setBounds(0, 20.0);
	sigma.setPriorDistribution("Uniform", 0.0, +20.0);
	addParameter(sigma);
}

void Korali::Likelihood::setReferenceData(size_t nData, double* referenceData)
{
	_nData = nData;
	_referenceData = referenceData;
	_referenceDataSet = true;
}

double Korali::Likelihood::evaluateFitness(double* sample)
{
	double sigma = sample[0];
	double* parameters = &sample[1];
  double* measuredData = _modelFunction(parameters);

	return -Korali::GaussianDistribution::logLikelihood(sigma, _nData, _referenceData, measuredData);
}

Korali::Posterior::Posterior(double* (*modelFunction) (double*), size_t seed) : Korali::Likelihood::Likelihood(modelFunction, seed)
{
	_parameters[0].setPriorDistribution("Uniform", 0.0, 20.0);
}

double Korali::Posterior::evaluateFitness(double* sample)
{
	double sigma = sample[0];
	double* parameters = &sample[1];
  double* measuredData = _modelFunction(parameters);

  double posterior = Korali::GaussianDistribution::logLikelihood(sigma, _nData, _referenceData, measuredData);
  double prev = posterior;
  //for (int i = 0; i < _parameterCount; i++) printf("%d) %f - %f\n", i, sample[i], log(_parameters[i]._prior->getDensity(sample[i])));
  for (int i = 0; i < _parameterCount; i++) posterior -= log(_parameters[i].getDensity(sample[i]));

  //printf("Before: %f, After: %f\n", prev, posterior);

  return -posterior;
}
