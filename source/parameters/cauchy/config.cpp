#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Cauchy::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Cauchy::setConfiguration(json js)
{
  _loc   = 0.0;
  _scale = 1.0;

  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("") != dist.end()) if (dist["Location"].is_number())
    { _loc = dist["Location"]; dist.erase("Location"); }
    if (dist.find("Scale") != dist.end()) if (dist["Scale"].is_number())
    { _scale = dist["Scale"]; dist.erase("Scale"); }
  }

  this->Korali::Parameter::Base::setConfiguration(js);
}
