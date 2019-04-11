#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Uniform::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();

 js["Type"] = "Uniform";

 return js;
}

void Korali::Parameter::Uniform::setConfiguration(json& js)
{
 this->Korali::Parameter::Base::setConfiguration(js);

 // Configuration goes here
}
