#include "korali.h"

/************************************************************************/
/*                  Constructor / Destructor Methods                    */
/************************************************************************/

Korali::Conduit::Base::Base(nlohmann::json& js)
{
 setConfiguration(js);
}

Korali::Conduit::Base::~Base()
{

}

/************************************************************************/
/*                    Configuration Methods                             */
/************************************************************************/

nlohmann::json Korali::Conduit::Base::getConfiguration()
{
 auto js = nlohmann::json();
 return js;
}

void Korali::Conduit::Base::setConfiguration(nlohmann::json js)
{
}

/************************************************************************/
/*                    Functional Methods                                */
/************************************************************************/

