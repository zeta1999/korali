#include "korali.h"

bool Korali::Problem::Base::isSampleOutsideBounds(double* sample)
{
  for (size_t i = 0; i < _k->N; i++)
  	if (sample[i] < _k->_parameters[i]->_lowerBound || sample[i] > _k->_parameters[i]->_upperBound) return true;
  return false;
}
