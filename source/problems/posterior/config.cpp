#include "korali.h"

using json = nlohmann::json;

json Korali::Problem::Posterior::getConfiguration()
{
 auto js = this->Korali::Problem::Likelihood::getConfiguration();

 js["Objective"] = "Posterior";

 return js;
}

void Korali::Problem::Posterior::setConfiguration(json& js)
{
  // Configuration goes here.

 this->Korali::Problem::Likelihood::setConfiguration(js);
}
