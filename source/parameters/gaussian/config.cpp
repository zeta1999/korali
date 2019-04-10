#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Gaussian::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Gaussian::setConfiguration(json js)
{
  _mean = 0.0;
  _sigma = 1.0;

  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("Mean") != dist.end()) if (dist["Mean"].is_number())
    { _mean = dist["Mean"]; dist.erase("Mean"); }
    if (dist.find("Sigma") != dist.end()) if (dist["Sigma"].is_number())
    { _sigma = dist["Sigma"]; dist.erase("Sigma"); }
  }

	this->Korali::Parameter::Base::setConfiguration(js);
}
