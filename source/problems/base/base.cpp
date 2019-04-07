#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Base::Base()
{
}

void Korali::Problem::Base::initialize()
{
}

json Korali::Problem::Base::getConfiguration()
{
 auto js = json();

 return js;
}

void Korali::Problem::Base::setConfiguration(json js)
{
}

bool Korali::Problem::Base::isSampleOutsideBounds(double* sample)
{
  for (size_t i = 0; i < _k->N; i++)
  	if (sample[i] < _k->_parameters[i]->_lowerBound || sample[i] > _k->_parameters[i]->_upperBound) return true;
  return false;
}
