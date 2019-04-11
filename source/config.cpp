#include "korali.h"

using json = nlohmann::json;

json Korali::Engine::getConfiguration()
{
 auto js = json();

 js["Seed"] = _seed;

 if (_verbosity == KORALI_SILENT)   js["Verbosity"] = "Silent";
 if (_verbosity == KORALI_MINIMAL)  js["Verbosity"] = "Minimal";
 if (_verbosity == KORALI_NORMAL)   js["Verbosity"] = "Normal";
 if (_verbosity == KORALI_DETAILED) js["Verbosity"] = "Detailed";

 js["Report Frequency"] = _reportFrequency;

 for (size_t i = 0; i < N; i++) js["Parameters"][i] = _parameters[i]->getConfiguration();
 js["Problem"] = _problem->getConfiguration();
 js["Solver"] = _solver->getConfiguration();
 js["Conduit"] = _conduit->getConfiguration();

 return js;
}

void Korali::Engine::setConfiguration(json js)
{
 // Configure Korali Engine
 std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::nanoseconds(0));
 _seed  = std::chrono::nanoseconds(now_c).count();
 _seed = consume(js, { "Seed" }, KORALI_NUMBER, "0");
 gsl_rng_env_setup();

 auto vString = consume(js, { "Verbosity" }, KORALI_STRING, "Normal");
 if (vString == "Silent")   _verbosity = KORALI_SILENT;
 if (vString == "Minimal")  _verbosity = KORALI_MINIMAL;
 if (vString == "Normal")   _verbosity = KORALI_NORMAL;
 if (vString == "Detailed") _verbosity = KORALI_DETAILED;

 _reportFrequency = consume(js, { "Report Frequency" }, KORALI_NUMBER, "1");

 // Configure Parameters
 std::vector<Korali::Parameter::Base*> tmp;

 if (isArray(js, { "Parameters" } ))
 for (size_t i = 0; i < js["Parameters"].size(); i++)
 {
  auto dString = consume(js["Parameters"][i], { "Distribution" }, KORALI_STRING);
  bool foundDistribution = false;
  if (dString == "Uniform")     { tmp.push_back(new Korali::Parameter::Uniform(js["Parameters"][i], _seed++));     foundDistribution = true; }
  if (dString == "Gaussian")    { tmp.push_back(new Korali::Parameter::Gaussian(js["Parameters"][i], _seed++));    foundDistribution = true; }
  if (dString == "Gamma")       { tmp.push_back(new Korali::Parameter::Gamma(js["Parameters"][i], _seed++));       foundDistribution = true; }
  if (dString == "Exponential") { tmp.push_back(new Korali::Parameter::Exponential(js["Parameters"][i], _seed++)); foundDistribution = true; }
  if (foundDistribution == false) { fprintf(stderr, "[Korali] Error: Incorrect or missing distribution for parameter %lu.\n", i); exit(-1); }
 }

 if (tmp.size() == 0) { fprintf(stderr, "[Korali] Error: Incorrect or undefined parameters.\n"); exit(-1); }

 _statisticalParameterCount = 0;
 _computationalParameterCount = 0;
 for (size_t i = 0; i < tmp.size(); i++) if (tmp[i]->_type == KORALI_COMPUTATIONAL) { _parameters.push_back(tmp[i]); _computationalParameterCount++; }
 for (size_t i = 0; i < tmp.size(); i++) if (tmp[i]->_type == KORALI_STATISTICAL)   { _parameters.push_back(tmp[i]); _statisticalParameterCount++; };
 N = _parameters.size();

 // Configure Problem
 bool foundProblem = false;
 auto pString =  consume(js, { "Problem", "Objective" }, KORALI_STRING);
 if (pString == "Direct Evaluation") { _problem = new Korali::Problem::Direct(js["Problem"]);     foundProblem = true; }
 if (pString == "Likelihood")        { _problem = new Korali::Problem::Likelihood(js["Problem"]); foundProblem = true; }
 if (pString == "Posterior")         { _problem = new Korali::Problem::Posterior(js["Problem"]);  foundProblem = true; }
 if (foundProblem == false) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Problem."); exit(-1); }

 // Configure Conduit
 std::string conduitString = "Sequential";

 #ifdef _KORALI_USE_UPCXX
 upcxx::init();
 if (upcxx::rank_n() > 1) conduitString = "UPC++";
 #endif

 conduitString = consume(js, { "Conduit", "Type" }, KORALI_STRING, conduitString);
 _conduit = NULL;

 if (conduitString == "Sequential")
 {
  _conduit = new Korali::Conduit::Single(js["Conduit"]);
 }

 if (conduitString == "UPC++")
 {
  #ifdef _KORALI_USE_UPCXX
   _conduit = new Korali::Conduit::UPCXX(js["Conduit"]);
  #else
   fprintf(stderr, "[Korali] Error: UPC++ conduit is not properly configured.\n");
   fprintf(stderr, "[Korali] Reinstall Korali with the proper configuration to support UPC++.\n");
   exit(-1);
  #endif
 }

 if (conduitString == "OpenMP")
 {
  #ifdef _KORALI_USE_OPENMP
   _conduit = new Korali::Conduit::OpenMP(js["Conduit"]);
  #else
   fprintf(stderr, "[Korali] Error: OpenMP conduit is not properly configured.\n");
   fprintf(stderr, "[Korali] Reinstall Korali with the proper configuration to support openMP.\n");
   exit(-1);
  #endif
 }

 if (_conduit == NULL)
 {
   fprintf(stderr, "[Korali] Error: Unrecognized or no conduit ('%s') selected.\n", conduitString.c_str());
   exit(-1);
 }

 // Configure Solver
 _solver = NULL;
 auto sString = consume(js, { "Solver", "Method" }, KORALI_STRING);
 if (sString == "CMA-ES") _solver = new Korali::Solver::CMAES(js["Solver"]);
 if (sString == "TMCMC")  _solver = new Korali::Solver::TMCMC(js["Solver"]);
 if (_solver == NULL) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Solver."); exit(-1); }

 if (isEmpty(js) == false)
 {
  fprintf(stderr, "[Korali] Error: Unrecognized Settings for Korali:\n");
  fprintf(stderr, "%s\n", js.dump(2).c_str());
  exit(-1);
 }
}

