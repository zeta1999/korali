#include "korali.h"

void Korali::Problem::Direct::initialize()
{
	this->Korali::Problem::Base::initialize();
}

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;
 return _k->_modelSingle(sample);
}




