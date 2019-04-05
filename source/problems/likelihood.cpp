#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Likelihood::Likelihood(void (*modelFunction) (double*, double*)) : Korali::Problem::Base::Base()
{
 _modelFunction = modelFunction;

 auto sigma = new Korali::Parameter::Uniform(0.0, +20.0);
 sigma->setName("Sigma");
 sigma->setBounds(0, +20.0);
 _k->addParameter(sigma);
}

double Korali::Problem::Likelihood::evaluateFitness(double* sample)
{
 if (_k->isSampleOutsideBounds(sample)) return -DBL_MAX;

 double sigma = sample[0];
 double* parameters = &sample[1];
 double* fitnessData = _k->_conduit->getFitnessArrayPointer();

 _modelFunction(parameters, fitnessData);

 return -Korali::Parameter::Gaussian::logLikelihood(sigma, _k->_referenceDataSize, _k->_referenceData, fitnessData);
}

json Korali::Problem::Likelihood::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();
 js["Type"] = "Likelihood";
 return js;
}

void Korali::Problem::Likelihood::setConfiguration(json js)
{
}
