#include "korali.h"

using json = nlohmann::json;

Korali::Solver::TMCMC::TMCMC(nlohmann::json& js) : Korali::Solver::Base::Base(js)
{
 setConfiguration(js);

 // Initializing Data Variables
 double *LCmem  = (double*) calloc (_s*_k->N*_k->N, sizeof(double));
 local_cov = (double**) calloc ( _s, sizeof(double*));
 for (size_t pos = 0; pos < _s; ++pos)
 {
  local_cov[pos] = LCmem + pos*_k->N*_k->N;
  for (size_t i = 0; i < _k->N; i++) local_cov[pos][i*_k->N+i] = 1;
 }

 // Initializing Run Variables
 _varianceCoefficient        = 0;
 _annealingRatio              = 0;
 _uniqueSelections = 0;
 _uniqueEntries = _s;
 _logEvidence  = 0;
 _acceptanceRate     = 1.0;
 nChains = _s;
 finishedChains = 0;
 _varianceCoefficient = std::numeric_limits<double>::infinity();
 _covarianceMatrix =  (double*) calloc (_k->N*_k->N, sizeof(double));
 _meanTheta =  (double*) calloc (_k->N+1, sizeof(double));

 // Initializing TMCMC Leaders
 ccPoints    = (double*) calloc (_k->N*_s, sizeof(double));
 ccFitness   = (double*) calloc (_s, sizeof(double));

 clPoints    = (double*) calloc (_k->N*_s, sizeof(double));
 clFitness   = (double*) calloc (_s, sizeof(double));

 chainPendingFitness = (bool*) calloc (_s, sizeof(bool));
 chainCurrentStep    = (size_t*) calloc (_s, sizeof(size_t));
 chainLength         = (size_t*) calloc (_s, sizeof(size_t));

 databaseEntries  = 0;
 databasePoints   = (double*) calloc (_k->N*_s, sizeof(double));
 databaseFitness  = (double*) calloc (_s, sizeof(double));

 for (size_t c = 0; c < _s; c++) chainCurrentStep[c] = 0;
 for (size_t c = 0; c < _s; c++) chainLength[c] = 1 + _burnIn;
 for (size_t c = 0; c < _s; c++) chainPendingFitness[c] = false;

 // Setting Chain-Specific Seeds
 range = gsl_rng_alloc (gsl_rng_default);
 gsl_rng_set(range, _k->_seed++);

 chainGSLRange = (gsl_rng**) calloc (_s, sizeof(gsl_rng*));
 for (size_t c = 0; c < _s; c++)
 {
  chainGSLRange[c] = gsl_rng_alloc (gsl_rng_default);
  gsl_rng_set(chainGSLRange[c], _k->_seed++);
 }

  // TODO: Ensure proper memory deallocation
}

Korali::Solver::TMCMC::~TMCMC()
{

}

json Korali::Solver::TMCMC::getConfiguration()
{
 auto js = this->Korali::Solver::Base::getConfiguration();

 js["Engine"] = "TMCMC";
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
