#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Gamma::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();

 js["Type"] = "Gamma";
 js["Rate"] = _rate;
 js["Shape"] = _shape;

 return js;
}

void Korali::Parameter::Gamma::setConfiguration(json& js)
{
 this->Korali::Parameter::Base::setConfiguration(js);

 _rate  = consume(js, { "Rate" }, KORALI_NUMBER);
 _shape = consume(js, { "Shape" }, KORALI_NUMBER);
}
