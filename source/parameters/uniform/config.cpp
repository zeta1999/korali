#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Uniform::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Uniform::setConfiguration(json js)
{
 this->Korali::Parameter::Base::setConfiguration(js);
}
