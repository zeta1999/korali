#ifndef _KORALI_JSON_H_
#define _KORALI_JSON_H_

#include "json/json.hpp"
#include <type_traits>
#include <vector>

namespace Korali
{

#pragma GCC diagnostic ignored "-Wunused-function"

static bool isDefined(nlohmann::json js, std::vector<std::string> settings)
{
 auto tmp = js;
 for (size_t i = 0; i < settings.size(); i++)
 {
  if (tmp.find(settings[i]) == tmp.end()) return false;
  tmp = tmp[settings[i]];
 }
 return true;
}

static bool isArray(nlohmann::json js, std::vector<std::string> settings)
{
 if (isDefined(js, settings))
 {
  auto tmp = js;
  for (size_t i = 0; i < settings.size(); i++)  tmp = tmp[settings[i]];
  if (tmp.is_array()) if (tmp.size() > 0) return true;
 }
 return false;
}

template <typename T> nlohmann::json consume(nlohmann::json& js, std::vector<std::string> settings, T def = std::string("__NO_DEFAULT") )
{
 bool hasDefault = true;

 if constexpr (std::is_same_v<T, std::string>) if (def == "__NO_DEFAULT") hasDefault = false;

 size_t sz =  settings.size();
 std::string fullOption = "";
 for(size_t i = 0; i < sz; i++) fullOption += ">" + settings[i];

 if (isDefined(js, settings))
 {
  nlohmann::json tmp = js;
  nlohmann::json* ptr = &tmp;
  for (size_t i = 0; i < sz-1; i++) ptr = &((*ptr)[settings[i]]);

  if ((*ptr)[settings[sz-1]].is_string())
  {
   nlohmann::json ret = (*ptr)[settings[sz-1]];
   ptr->erase(settings[sz-1]);
   js = tmp;
   return ret;
  }
  else
  {
   fprintf(stderr, "[Korali] Error: Passing non-string value to string-type option: %s.\n", fullOption.c_str());
   exit(-1);
  }
 }

 if (hasDefault == false)
 {
  fprintf(stderr, "[Korali] Error: No value passed for non-default string-type option: %s.\n", fullOption.c_str());
  exit(-1);
 }

 return nlohmann::json(def);
}

}

#endif // _KORALI_JSON_H_
