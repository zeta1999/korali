#ifdef _KORALI_USE_UPCXX

#include "korali.h"

using json = nlohmann::json;

json Korali::Conduit::UPCXX::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "UPC++"

 return js;
}

void Korali::Conduit::UPCXX::setConfiguration(json js)
{
  _rankId = 0;
  _rankCount = 1;
  _continueEvaluations = true;
}

#endif
