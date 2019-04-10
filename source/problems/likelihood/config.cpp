#include "korali.h"

using json = nlohmann::json;

json Korali::Problem::Likelihood::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();

 return js;
}

void Korali::Problem::Likelihood::setConfiguration(json& js)
{
 this->Korali::Problem::Base::setConfiguration(js);

 std::string model = consume(js, { "Model" }, KORALI_STRING);
 if (model != "Multiple")
 {
  fprintf(stderr, "[Korali] Error: Incorrect model for the Likelihood problem.\n");
  exit(-1);
 }

 auto ref = consume(js, { "Reference Data"}, KORALI_ARRAY);
 _referenceDataSize = ref.size();
 _referenceData = (double*) calloc (_referenceDataSize, sizeof(double));
 for (size_t i = 0; i < _referenceDataSize; i++) _referenceData[i] = ref[i];

 if (_referenceDataSize == 0)
 {
  fprintf(stderr, "[Korali] Error: No Reference Data set for Likelihood.\n");
  exit(-1);
 }
}
