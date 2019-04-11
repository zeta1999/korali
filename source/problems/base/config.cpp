#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Base::Base(nlohmann::json& js)
{
 setConfiguration(js);
}

Korali::Problem::Base::~Base()
{

}

json Korali::Problem::Base::getConfiguration()
{
 auto js = json();

 if (_model == KORALI_SINGLE)   js["Model"] = "Single";
 if (_model == KORALI_MULTIPLE) js["Model"] = "Multiple";

 return js;
}

void Korali::Problem::Base::setConfiguration(json& js)
{
 auto modelString = consume(js, { "Model" }, KORALI_STRING);
 if (modelString == "Single")   _model = KORALI_SINGLE;
 if (modelString == "Multiple") _model = KORALI_MULTIPLE;
}
