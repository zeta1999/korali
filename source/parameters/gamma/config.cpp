#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Gamma::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Gamma::setConfiguration(json js)
{
	_rate = 1.0;
	_shape = 1.0;

  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("Shape") != dist.end()) if (dist["Shape"].is_number())
    { _shape = dist["Shape"]; dist.erase("Shape"); }
    if (dist.find("Rate") != dist.end()) if (dist["Rate"].is_number())
    { _rate = dist["Rate"]; dist.erase("Rate"); }
  }

	this->Korali::Parameter::Base::setConfiguration(js);
}
