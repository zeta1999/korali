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

 bool correctModel = false;
 if (js.find("Model") != js.end()) if (js["Model"].is_string())
 { if (js["Model"] == "Single") correctModel = true; }

 if (correctModel == false)
 {
	 fprintf(stderr, "[Korali] Error: Incorrect model for the Direct Evaluation problem.\n");
	 exit(-1);
 }
}
