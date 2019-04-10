#include "korali.h"

using json = nlohmann::json;

json Korali::Problem::Likelihood::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();

 return js;
}

void Korali::Problem::Likelihood::setConfiguration(json js)
{
 _referenceData = NULL;
 _referenceDataSize = 0;

 this->Korali::Problem::Base::setConfiguration(js);

 if (js.find("Reference Data") != js.end()) if (js["Reference Data"].is_array())
 {
  auto ref = js["Reference Data"];
  _referenceDataSize = ref.size();
  _referenceData = (double*) calloc (_referenceDataSize, sizeof(double));
  for (size_t i = 0; i < _referenceDataSize; i++) _referenceData[i] = ref[i];
 }

 bool correctModel = false;
 if (js.find("Model") != js.end()) if (js["Model"].is_string())
 { if (js["Model"] == "Multiple") correctModel = true; }

 if (correctModel == false)
 {
  fprintf(stderr, "[Korali] Error: Incorrect model for the Likelihood problem.\n");
  exit(-1);
 }

 if (_referenceDataSize == 0)
 {
  fprintf(stderr, "[Korali] Error: No Reference Data set for Likelihood.\n");
  exit(-1);
 }
}
