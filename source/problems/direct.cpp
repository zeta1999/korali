#include "problems/direct.h"

Korali::Problem::Direct::Direct(double (*modelFunction) (double*), size_t seed) : Korali::Problem::Base::Base(seed)
{
	_modelFunction = modelFunction;
}

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
  return _modelFunction(sample);
}

