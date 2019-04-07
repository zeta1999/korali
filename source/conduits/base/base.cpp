#include "korali.h"

using json = nlohmann::json;

json Korali::Conduit::Base::getConfiguration()
{
 auto js = json();
 return js;
}

void Korali::Conduit::Base::setConfiguration(json js)
{
}
