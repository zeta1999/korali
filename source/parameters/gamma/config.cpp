#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Gamma::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Gamma::setConfiguration(json& js)
{
 this->Korali::Parameter::Base::setConfiguration(js);

 _rate  = consume(js, { "Rate" }, KORALI_NUMBER);
 _shape = consume(js, { "Shape" }, KORALI_NUMBER);
}
