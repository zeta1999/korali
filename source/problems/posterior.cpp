#include "problems/posterior.h"
#include "parameters/gaussian.h"

using json = nlohmann::json;

Korali::Problem::Posterior::Posterior(void (*modelFunction) (double*, double*), size_t seed) : Korali::Problem::Likelihood::Likelihood(modelFunction, seed)
{
}

double Korali::Problem::Posterior::evaluateFitness(double* sample)
{
  double posterior = Korali::Problem::Likelihood::evaluateFitness(sample);
  for (size_t i = 0; i < _parameterCount; i++) posterior += log(_parameters[i]->getDensity(sample[i]));

  return posterior;
}

json Korali::Problem::Posterior::serialize()
{
 auto j = this->Korali::Problem::Likelihood::serialize();
 j["Type"] = "Posterior";

 return j;
}
