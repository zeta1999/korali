#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Uniform::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Uniform::setConfiguration(json js)
{
  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("Minimum") != dist.end()) if (dist["Minimum"].is_number())
    { _min = dist["Minimum"]; dist.erase("Minimum"); }
    if (dist.find("Maximum") != dist.end()) if (dist["Maximum"].is_number())
    { _max = dist["Maximum"]; dist.erase("Maximum"); }
  }

  _lowerBound = _min;
  _upperBound = _max;

	this->Korali::Parameter::Base::setConfiguration(js);
}
