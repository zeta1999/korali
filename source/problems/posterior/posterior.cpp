#include "korali.h"

double Korali::Problem::Posterior::evaluateFitness(double* sample)
{
  double posterior = Korali::Problem::Likelihood::evaluateFitness(sample);
  for (size_t i = 0; i < _k->N; i++) posterior += log(_k->_parameters[i]->getDensity(sample[i]));

  return posterior;
}

