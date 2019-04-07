#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Direct::Direct() : Korali::Problem::Base::Base()
{
}

void Korali::Problem::Direct::initialize()
{
	this->Korali::Problem::Base::initialize();
}

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
 if (_k->isSampleOutsideBounds(sample)) return -DBL_MAX;
 return _k->_modelSingle(sample);
}

json Korali::Problem::Direct::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();
 return js;
}

void Korali::Problem::Direct::setConfiguration(json js)
{
	this->Korali::Problem::Base::setConfiguration(js);
}


