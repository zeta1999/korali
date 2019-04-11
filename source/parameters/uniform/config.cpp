#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Uniform::Uniform(nlohmann::json& js, int seed) : Korali::Parameter::Base::Base(js, seed)
{
 setConfiguration(js);
}

Korali::Parameter::Uniform::~Uniform()
{

}

json Korali::Parameter::Uniform::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();

 js["Type"] = "Uniform";

 return js;
}

void Korali::Parameter::Uniform::setConfiguration(json& js)
{
}
