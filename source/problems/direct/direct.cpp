#include "korali.h"

void Korali::Problem::Direct::initialize()
{
 this->Korali::Problem::Base::initialize();

 if (_k->_statisticalParameterCount != 0) { fprintf(stderr, "[Korali] Error: Direct Evaluation problem requires 0 statistical parameters.\n"); exit(-1); }
}

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;
 return _k->_modelSingle(sample);
}




