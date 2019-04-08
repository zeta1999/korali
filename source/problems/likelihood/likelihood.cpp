#include "korali.h"

void Korali::Problem::Likelihood::initialize()
{
 this->Korali::Problem::Base::initialize();

 if (_k->_statisticalParameterCount != 1) { fprintf(stderr, "[Korali] Error: Likelihood problem requires 1 statistical parameter.\n"); exit(-1); }

}

double Korali::Problem::Likelihood::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;

 double sigma = sample[_k->_computationalParameterCount];
 double fitnessData[_referenceDataSize];

 _k->_modelMultiple(sample, fitnessData);

 return -Korali::Parameter::Gaussian::logLikelihood(sigma, _referenceDataSize, _referenceData, fitnessData);
}

