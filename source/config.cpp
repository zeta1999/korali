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


bool Korali::isDefined(json js, std::vector<std::string> settings)
{
 auto tmp = js;
 for (int i = 0; i < settings.size(); i++)
 {
  if (tmp.find(settings[i]) == tmp.end()) return false;
  tmp = tmp[settings[i]];
 }
 return true;
}

bool Korali::isArray(json js, std::vector<std::string> settings)
{
 if (isDefined(js, settings))
 {
  auto tmp = js;
  for (int i = 0; i < settings.size(); i++)  tmp = tmp[settings[i]];
  if (tmp.is_array()) if (tmp.size() > 0) return true;
 }
 return false;
}

json Korali::consumeString(json& js, std::vector<std::string> settings)
{
 if (isDefined(js, settings))
 {
  json tmp = js;
  json* ptr = &tmp;
  for (int i = 0; i < settings.size()-1; i++) ptr = &((*ptr)[settings[i]]);
  if ((*ptr)[settings[settings.size()-1]].is_string())
  {
   json ret = (*ptr)[settings[settings.size()-1]];
   ptr->erase(settings[settings.size()-1]);
   js = tmp;
   return ret;
  }
 }
 return json("");
}
