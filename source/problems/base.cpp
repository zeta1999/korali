#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Base::Base()
{
}

json Korali::Problem::Base::getConfiguration()
{
 auto js = json();

 return js;
}

void Korali::Problem::Base::setConfiguration(json js)
{
}
