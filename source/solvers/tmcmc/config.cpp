#include "korali.h"

using json = nlohmann::json;

json Korali::Solver::TMCMC::getConfiguration()
{
  auto js = this->Korali::Solver::Base::getConfiguration();

  return js;
}

void Korali::Solver::TMCMC::setConfiguration(json& js)
{
 _tolCOV  = 1;
 _minStep = 1e-9;
 _bbeta   = 0.005;
 _useLocalCov = false;
 _maxGens = 200;
 _s = 1000;
 _burnIn = 0;
 _currentGeneration = 0;

  this->Korali::Solver::Base::setConfiguration(js);

  if (js.find("currentGeneration") != js.end()) if (js["currentGeneration"].is_number())
  { _currentGeneration = js["currentGeneration"]; js.erase("currentGeneration"); }

 if (js.find("Population Size") != js.end()) if (js["Population Size"].is_number())
 { _s = js["Population Size"]; js.erase("Population Size"); }

  if (js.find("Covariance Tolerance") != js.end()) if (js["Covariance Tolerance"].is_number())
 { _tolCOV = js["Covariance Tolerance"]; js.erase("Covariance Tolerance"); }

 if (js.find("Min Rho Update") != js.end()) if (js["Min Rho Update"].is_number())
 { _minStep = js["Min Rho Update"]; js.erase("Min Rho Update"); }

 if (js.find("Covariance Scaling") != js.end()) if (js["Covariance Scaling"].is_number())
 { _bbeta = js["Covariance Scaling"]; js.erase("Covariance Scaling"); }

 if (js.find("Use Local Covariance") != js.end()) if (js["Use Local Covariance"].is_boolean())
 { _useLocalCov = js["Use Local Covariance"]; js.erase("Use Local Covariance"); }

 if (js.find("Burn In") != js.end()) if (js["Burn In"].is_number())
 { _burnIn = js["Burn In"]; js.erase("Burn In"); }

  if (js.find("TerminationCriteria") != js.end())
  {
    json term = js["TerminationCriteria"];
   if (term.find("Max Generations") != term.end()) if (term["Max Generations"].is_number())
    { _maxGens = term["Max Generations"]; term.erase("Max Generations"); }
  }
}

json Korali::Solver::TMCMC::getState()
{
  auto js = this->Korali::Solver::Base::getState();

  return js;
}

void Korali::Solver::TMCMC::setState(json js)
{
  this->Korali::Solver::Base::setState(js);
}
