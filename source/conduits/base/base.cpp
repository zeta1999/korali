#include "korali.h"

Korali::Conduit::Base::Base(nlohmann::json& js)
{
 setConfiguration(js);
}

Korali::Conduit::Base::~Base()
{

}
