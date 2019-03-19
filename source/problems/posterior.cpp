#include "problems/posterior.h"
#include <stdlib.h>
#include <chrono>

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
