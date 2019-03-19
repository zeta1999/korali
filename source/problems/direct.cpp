#include "problems/direct.h"
#include <stdlib.h>
#include <chrono>

Korali::Direct::Direct(double (*modelFunction) (double*), size_t seed) : Korali::BaseProblem::BaseProblem(seed)
{
	_modelFunction = modelFunction;
}

double Korali::Direct::evaluateFitness(double* sample)
{
  return _modelFunction(sample);
}

