#include "korali.h"

using json = nlohmann::json;

json Korali::Problem::Base::getConfiguration()
{
 auto js = json();

 return js;
}

void Korali::Problem::Base::setConfiguration(json js)
{
}
