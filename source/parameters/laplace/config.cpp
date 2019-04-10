#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Laplace::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Laplace::setConfiguration(json js)
{
  _mean = 0.0;
  _width = 1.0;

  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("Mean") != dist.end()) if (dist["Mean"].is_number())
    { _mean = dist["Mean"]; dist.erase("Mean"); }
    if (dist.find("Width") != dist.end()) if (dist["Width"].is_number())
    { _width = dist["Width"]; dist.erase("Width"); }
  }

	this->Korali::Parameter::Base::setConfiguration(js);
}
