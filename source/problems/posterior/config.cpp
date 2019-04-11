#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Posterior::Posterior(nlohmann::json& js) : Korali::Problem::Likelihood::Likelihood(js)
{
 setConfiguration(js);
}

Korali::Problem::Posterior::~Posterior()
{
}

json Korali::Problem::Posterior::getConfiguration()
{
 auto js = this->Korali::Problem::Likelihood::getConfiguration();

 js["Objective"] = "Posterior";

 return js;
}

void Korali::Problem::Posterior::setConfiguration(json& js)
{
}
