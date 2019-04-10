#include "korali.h"

using json = nlohmann::json;

json Korali::Problem::Direct::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();
 return js;
}

void Korali::Problem::Direct::setConfiguration(json js)
{
 this->Korali::Problem::Base::setConfiguration(js);

 std::string model = consume(js, { "Model" }, KORALI_STRING);
 if (model != "Single")
 {
  fprintf(stderr, "[Korali] Error: Incorrect model for the Direct Evaluation problem.\n");
  exit(-1);
 }
}
