#include "korali.h"

using json = nlohmann::json;

Korali::Solver::Base::Base(nlohmann::json& js)
{
 setConfiguration(js);
}

Korali::Solver::Base::~Base()
{

}

json Korali::Solver::Base::getConfiguration()
{
 auto js = json();

 return js;
}

void Korali::Solver::Base::setConfiguration(json& js)
{

}

json Korali::Solver::Base::getState()
{
 auto js = json();

 return js;
}

void Korali::Solver::Base::setState(json js)
{

}
