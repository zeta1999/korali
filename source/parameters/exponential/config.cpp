#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Exponential::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Exponential::setConfiguration(json js)
{
	_mean = 1.0;

  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("Mean") != dist.end()) if (dist["Mean"].is_number())
    { _mean = dist["Mean"]; dist.erase("Mean"); }
  }

	this->Korali::Parameter::Base::setConfiguration(js);
}
