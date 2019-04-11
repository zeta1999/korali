#include "korali.h"

using json = nlohmann::json;

json Korali::Solver::TMCMC::getConfiguration()
{
 auto js = this->Korali::Solver::Base::getConfiguration();

 js["Population Size"] = _s;
 js["Current Generation"] = _currentGeneration;
 js["Covariance Tolerance"] = _tolCOV;
 js["Min Rho Update"] = _minStep;
 js["Covariance Scaling"] = _bbeta;
 js["Use Local Covariance"] = _useLocalCov;
 js["Burn In"] = _burnIn;
 js["Termination Criteria"]["Max Generations"] = _maxGens;;

 return js;
}

void Korali::Solver::TMCMC::setConfiguration(json& js)
{
 this->Korali::Solver::Base::setConfiguration(js);

 _currentGeneration = consume(js, { "Current Generation" }, KORALI_NUMBER, std::to_string(0));
 _s                 = consume(js, { "Population Size" }, KORALI_NUMBER);
 _tolCOV            = consume(js, { "Covariance Tolerance" }, KORALI_NUMBER, std::to_string(1.0));
 _minStep           = consume(js, { "Min Rho Update" }, KORALI_NUMBER, std::to_string(1e-9));
 _bbeta             = consume(js, { "Covariance Scaling" }, KORALI_NUMBER, std::to_string(0.005));
 _useLocalCov       = consume(js, { "Use Local Covariance" }, KORALI_BOOLEAN, "false");
 _burnIn            = consume(js, { "Burn In" }, KORALI_NUMBER, std::to_string(0));
 _maxGens           = consume(js, { "Termination Criteria", "Max Generations" }, KORALI_NUMBER, std::to_string(20));
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
