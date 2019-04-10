#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Exponential::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Exponential::setConfiguration(json js)
{
 this->Korali::Parameter::Base::setConfiguration(js);

 _mean = consume(js, { "Mean" }, KORALI_NUMBER);
}
