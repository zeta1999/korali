#ifdef _KORALI_USE_OPENMP

#include "korali.h"

using json = nlohmann::json;

Korali::Conduit::OpenMP::OpenMP(nlohmann::json& js) : Korali::Conduit::Base::Base(js)
{
 setConfiguration(js);
}

Korali::Conduit::OpenMP::~OpenMP()
{

}

json Korali::Conduit::OpenMP::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "OpenMP"

 return js;
}

void Korali::Conduit::OpenMP::setConfiguration(json js)
{
 _threadCount = 1;
 _continueEvaluations = true;
}

#endif
