#include "korali.h"

using json = nlohmann::json;

json Korali::Problem::Posterior::getConfiguration()
{
 auto js = this->Korali::Problem::Likelihood::getConfiguration();
 return js;
}

void Korali::Problem::Posterior::setConfiguration(json js)
{
	this->Korali::Problem::Likelihood::setConfiguration(js);
}
