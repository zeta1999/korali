#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Gaussian::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Gaussian::setConfiguration(json& js)
{
 this->Korali::Parameter::Base::setConfiguration(js);

 _mean  = consume(js, { "Mean" }, KORALI_NUMBER);
 _sigma = consume(js, { "Sigma" }, KORALI_NUMBER);
}
