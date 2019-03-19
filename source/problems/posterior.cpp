#include "problems/posterior.h"
#include "parameters/gaussian.h"

Korali::Posterior::Posterior(double* (*modelFunction) (double*), size_t seed) : Korali::Likelihood::Likelihood(modelFunction, seed)
{
}

double Korali::Posterior::evaluateFitness(double* sample)
{
	double sigma = sample[0];
	double* parameters = &sample[1];
  double* measuredData = _modelFunction(parameters);

  double posterior = Korali::Parameter::Gaussian::logLikelihood(sigma, _nData, _referenceData, measuredData);
  double prev = posterior;
  //for (int i = 0; i < _parameterCount; i++) printf("%d) %f - %f\n", i, sample[i], log(_parameters[i]->getDensity(sample[i])));
  for (int i = 0; i < _parameterCount; i++) posterior -= log(_parameters[i]->getDensity(sample[i]));

  return -posterior;
}
