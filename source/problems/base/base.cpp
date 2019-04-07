#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Base::Base()
{
}

void Korali::Problem::Base::initialize()
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
