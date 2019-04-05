#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Direct::Direct(double (*modelFunction) (double*)) : Korali::Problem::Base::Base()
{
 _modelFunction = modelFunction;
}

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
 if (_k->isSampleOutsideBounds(sample)) return -DBL_MAX;
 return _modelFunction(sample);
}

json Korali::Problem::Direct::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();
 js["Type"] = "Direct Evaluation";
 return js;
}

void Korali::Problem::Direct::setConfiguration(json js)
{
}


