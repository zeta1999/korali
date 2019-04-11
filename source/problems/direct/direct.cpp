#include "korali.h"

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;
 return _k->_modelSingle(sample);
}




