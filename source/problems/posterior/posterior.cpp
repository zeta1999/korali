#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Posterior::Posterior() : Korali::Problem::Likelihood::Likelihood()
{
}

void Korali::Problem::Posterior::initialize()
{
	this->Korali::Problem::Base::initialize();
}

double Korali::Problem::Posterior::evaluateFitness(double* sample)
{
  double posterior = Korali::Problem::Likelihood::evaluateFitness(sample);
  for (size_t i = 0; i < _k->N; i++) posterior += log(_k->_parameters[i]->getDensity(sample[i]));

  return posterior;
}

json Korali::Problem::Posterior::getConfiguration()
{
 auto js = this->Korali::Problem::Likelihood::getConfiguration();
 return js;
}

void Korali::Problem::Posterior::setConfiguration(json js)
{
	this->Korali::Problem::Likelihood::setConfiguration(js);
}


