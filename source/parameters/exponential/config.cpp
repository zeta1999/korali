#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Exponential::Exponential(nlohmann::json& js, int seed) : Korali::Parameter::Base::Base(js, seed)
{
 setConfiguration(js);
}

Korali::Parameter::Exponential::~Exponential()
{

}

json Korali::Parameter::Exponential::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();

 js["Type"] = "Exponential";
 js["Mean"] = _mean;

 return js;
}

void Korali::Parameter::Exponential::setConfiguration(json& js)
{
 _mean = consume(js, { "Mean" }, KORALI_NUMBER);
}
