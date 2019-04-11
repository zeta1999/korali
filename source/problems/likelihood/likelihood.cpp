#include "korali.h"

double Korali::Problem::Likelihood::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;

 double sigma = sample[_k->_computationalParameterCount];
 double fitnessData[_referenceDataSize];

 _k->_modelMultiple(sample, fitnessData);

 return -Korali::Parameter::Gaussian::logLikelihood(sigma, _referenceDataSize, _referenceData, fitnessData);
}

