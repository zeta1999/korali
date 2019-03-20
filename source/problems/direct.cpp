#include "problems/direct.h"

Korali::Problem::Direct::Direct(double (*modelFunction) (double*), size_t seed) : Korali::Problem::Base::Base(seed)
{
	_modelFunction = modelFunction;
}

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
  if (isSampleOutsideBounds(sample)) return -DBL_MAX;

  return _modelFunction(sample);
}

