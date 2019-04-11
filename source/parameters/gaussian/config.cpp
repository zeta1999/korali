#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Gaussian::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();

 js["Type"] = "Gaussian";
 js["Mean"] = _mean;
 js["Sigma"] = _sigma;

 return js;
}

void Korali::Parameter::Gaussian::setConfiguration(json& js)
{
 this->Korali::Parameter::Base::setConfiguration(js);

 _mean  = consume(js, { "Mean" }, KORALI_NUMBER);
 _sigma = consume(js, { "Sigma" }, KORALI_NUMBER);
}
