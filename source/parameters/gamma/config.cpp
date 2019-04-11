#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Gamma::Gamma(nlohmann::json& js, int seed) : Korali::Parameter::Base::Base(js, seed)
{
 setConfiguration(js);
}

Korali::Parameter::Gamma::~Gamma()
{

}

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
 _rate  = consume(js, { "Rate" }, KORALI_NUMBER);
 _shape = consume(js, { "Shape" }, KORALI_NUMBER);
}
