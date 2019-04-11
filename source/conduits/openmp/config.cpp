#ifdef _KORALI_USE_OPENMP

#include "korali.h"

using json = nlohmann::json;

json Korali::Conduit::Single::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "OpenMP"

 return js;
}

void Korali::Conduit::Single::setConfiguration(json js)
{
 _threadCount = 1;
 _continueEvaluations = true;

 this->Korali::Conduit::Base::setConfiguration(js);
}

#endif
