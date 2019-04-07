#include "korali.h"

void Korali::Problem::Likelihood::initialize()
{
	// auto sigma = new Korali::Parameter::Uniform(0.0, +20.0);
	// sigma->setName("Sigma");
	// sigma->setBounds(0, +20.0);
	// _k->addParameter(sigma);

	this->Korali::Problem::Base::initialize();
}

double Korali::Problem::Likelihood::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;

 double sigma = sample[0];
 double* parameters = &sample[1];
 double fitnessData[_referenceDataSize];

 _k->_modelMultiple(parameters, fitnessData);

 double likelihood = -Korali::Parameter::Gaussian::logLikelihood(sigma, _referenceDataSize, _referenceData, fitnessData);

 return likelihood;
}

