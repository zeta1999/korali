#include "korali.h"

using json = nlohmann::json;

json Korali::Conduit::Single::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "Single";

 return js;
}

void Korali::Conduit::Single::setConfiguration(json js)
{
 this->Korali::Conduit::Base::setConfiguration(js);
}
