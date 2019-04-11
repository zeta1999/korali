#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Posterior::Posterior(nlohmann::json& js) : Korali::Problem::Likelihood::Likelihood(js)
{
 setConfiguration(js);
}

Korali::Problem::Posterior::~Posterior()
{
}

json Korali::Problem::Posterior::getConfiguration()
{
 auto js = this->Korali::Problem::Likelihood::getConfiguration();

 js["Objective"] = "Posterior";

 return js;
}

void Korali::Problem::Posterior::setConfiguration(json& js)
{
}

double Korali::Problem::Posterior::evaluateFitness(double* sample)
{
  double posterior = Korali::Problem::Likelihood::evaluateFitness(sample);
  for (size_t i = 0; i < _k->N; i++) posterior += log(_k->_parameters[i]->getDensity(sample[i]));

  return posterior;
}

