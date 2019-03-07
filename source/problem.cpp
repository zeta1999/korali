#include "problem.h"
#include <time.h>
#include <stdlib.h>

Korali::Problem::Problem(size_t seed)
{
	_seed = seed;
	if (_seed == -1) _seed = clock();
	_parameterCount = 0;
  gsl_rng_env_setup();
}

void Korali::Problem::addParameter(Parameter p)
{
	if(p._name == "") p._name = "Parameter" + std::to_string(_parameterCount);
	_parameters.push_back(p);
	p.setSeed(_seed++);
	_parameterCount = _parameters.size();
}

double Korali::Problem::getPriorsLogProbabilityDensity(double *x)
{
  double logp = 0.0;
  for (int i = 0; i < _parameterCount; i++) logp += _parameters[i].getDensityLog(x[i]);
  return logp;
}

double Korali::Problem::getPriorsProbabilityDensity(double *x)
{
  double dp = 1.0;
  for (int i = 0; i < _parameterCount; i++) dp *= _parameters[i].getDensity(x[i]);
  return dp;
}

Korali::DirectEvaluation::DirectEvaluation(double (*modelFunction) (double*), size_t seed) : Korali::Problem::Problem(seed)
{
	_modelFunction = modelFunction;
}

double Korali::DirectEvaluation::evaluateFitness(double* sample)
{
  return _modelFunction(sample);
}

Korali::Likelihood::Likelihood(double* (*modelFunction) (double*, void*), size_t seed) : Korali::Problem::Problem(seed)
{
	_referenceData = 0;
	_modelData = 0;
	_nData = 0;
	_modelFunction = modelFunction;
	_modelDataSet = false;
	_referenceDataSet = false;

	Korali::Parameter sigma("Sigma");
	sigma.setBounds(0, 20.0);
	addParameter(sigma);
}

void Korali::Likelihood::setModelData(void* modelData)
{
	_modelData = modelData;
	_modelDataSet = true;
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
  double* measuredData = _modelFunction(parameters, _modelData);

	return Korali::GaussianDistribution::logLikelihood(sigma, _nData, _referenceData, measuredData);
}

bool Korali::DirectEvaluation::evaluateSettings(char* errorCode)
{
  for (int i = 0; i < _parameterCount; i++)
  	if (_parameters[i]._boundsSet == false)
		{
  		sprintf(errorCode, "[Korali] Error: Bounds for parameter %s have not been set.\n", _parameters[i]._name.c_str());
  		return true;
		}

  return false;
}

bool Korali::Likelihood::evaluateSettings(char* errorCode)
{
	if (_modelDataSet == false)
	{
		sprintf(errorCode, "[Korali] Error: Problem's model dataset not defined (use: setModelData()).\n");
		return true;
	}

	if (_referenceDataSet == false)
	{
		sprintf(errorCode, "[Korali] Error: Problem's reference dataset not defined (use: setReferenceData()).\n");
		return true;
	}

  for (int i = 0; i < _parameterCount; i++)
	if (_parameters[i]._boundsSet == false)
	{
		sprintf(errorCode, "[Korali] Error: Bounds for parameter \'%s\' have not been set.\n", _parameters[i]._name.c_str());
		return true;
	}

  return false;
}

Korali::Posterior::Posterior(double* (*modelFunction) (double*, void*), size_t seed) : Korali::Likelihood::Likelihood(modelFunction, seed)
{
	_parameters[0].setPriorDistribution("Uniform", 0.0, 20.0);
}

bool Korali::Posterior::evaluateSettings(char* errorCode)
{
	if (Likelihood::evaluateSettings(errorCode)) return true;

  for (int i = 0; i < _parameterCount; i++)
	if (_parameters[i]._priorSet == false)
	{
		sprintf(errorCode, "[Korali] Error: Prior for parameter \'%s\' have not been set.\n", _parameters[i]._name.c_str());
		return true;
	}

  return false;
}

double Korali::Posterior::evaluateFitness(double* sample)
{
	double sigma = sample[0];
	double* parameters = &sample[1];
  double* measuredData = _modelFunction(parameters, _modelData);

  double posterior = Korali::GaussianDistribution::logLikelihood(sigma, _nData, _referenceData, measuredData);
  double prev = posterior;
  //for (int i = 0; i < _parameterCount; i++) printf("%d) %f - %f\n", i, sample[i], log(_parameters[i]._prior->getDensity(sample[i])));
  for (int i = 0; i < _parameterCount; i++) posterior -= log(_parameters[i].getDensity(sample[i]));

  //printf("Before: %f, After: %f\n", prev, posterior);

  return posterior;
}
