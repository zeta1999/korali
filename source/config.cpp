#include "korali.h"

using json = nlohmann::json;

json Korali::Engine::getConfiguration()
{
 auto js = json();
 js["seed"] = _seed;
 js["verbosity"] = _verbosity;
 js["reportFrequency"] = _reportFrequency;
 return js;
}

void Korali::Engine::setConfiguration(json js)
{
 _seed              = js["seed"];
 _verbosity         = js["verbosity"];
 _reportFrequency   = js["reportFrequency"];
}
