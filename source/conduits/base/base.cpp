#include "korali.h"

/**************************************************************/
/*             Configuration / Initialization Code            */
/**************************************************************/

using json = nlohmann::json;

json Korali::Conduit::Base::getConfiguration()
{
 auto js = json();
 return js;
}

void Korali::Conduit::Base::setConfiguration(json js)
{
}

Korali::Conduit::Base::Base(nlohmann::json& js)
{
 setConfiguration(js);
}

Korali::Conduit::Base::~Base()
{

}

/**************************************************************/
/*                     Functional Code                        */
/**************************************************************/
