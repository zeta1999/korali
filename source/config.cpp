#include "korali.h"

using json = nlohmann::json;

json Korali::Engine::getConfiguration()
{
 auto js = json();
 return js;
}

void Korali::Engine::setConfiguration(json js)
{
 // Configure Korali Engine
 std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::nanoseconds(0));
 _seed  = std::chrono::nanoseconds(now_c).count();
 _seed = consume<size_t>(js, { "Seed" }, 0);
 gsl_rng_env_setup();

 auto vString = consume<std::string>(js, { "Verbosity" }, "Normal");
 if (vString == "Silent")   _verbosity = KORALI_SILENT;
 if (vString == "Minimal")  _verbosity = KORALI_MINIMAL;
 if (vString == "Normal")   _verbosity = KORALI_NORMAL;
 if (vString == "Detailed") _verbosity = KORALI_DETAILED;

 _reportFrequency = consume<size_t>(js, { "Report Frequency" }, 1);

 // Configure Parameters
 std::vector<Korali::Parameter::Base*> tmp;

 if (isArray(js, { "Parameters" } ))
 for (size_t i = 0; i < js["Parameters"].size(); i++)
 {
  auto dString = consume<std::string>(js["Parameters"][i], { "Distribution" });
  bool foundDistribution = false;
  if (dString == "Uniform")     { tmp.push_back(new Korali::Parameter::Uniform());     foundDistribution = true; }
  if (dString == "Gaussian")    { tmp.push_back(new Korali::Parameter::Gaussian());    foundDistribution = true; }
  if (dString == "Gamma")       { tmp.push_back(new Korali::Parameter::Gamma());       foundDistribution = true; }
  if (dString == "Exponential") { tmp.push_back(new Korali::Parameter::Exponential()); foundDistribution = true; }
  if (foundDistribution == false) { fprintf(stderr, "[Korali] Error: Incorrect or missing distribution for parameter %lu.\n", i); exit(-1); }
  tmp[i]->setConfiguration(js["Parameters"][i]);
 }

 if (tmp.size() == 0) { fprintf(stderr, "[Korali] Error: Incorrect or undefined parameters.\n"); exit(-1); }
 js.erase("Parameters");

 _statisticalParameterCount = 0;
 _computationalParameterCount = 0;
 for (size_t i = 0; i < tmp.size(); i++) if (tmp[i]->_type == KORALI_COMPUTATIONAL) { _parameters.push_back(tmp[i]); _computationalParameterCount++; }
 for (size_t i = 0; i < tmp.size(); i++) if (tmp[i]->_type == KORALI_STATISTICAL)   { _parameters.push_back(tmp[i]); _statisticalParameterCount++; };
 N = _parameters.size();

  // Configure Problem
 bool foundProblem = false;
 auto pString =  consume<std::string>(js, { "Problem", "Objective" });
 if (pString == "Direct Evaluation") { _problem = new Korali::Problem::Direct();     foundProblem = true; }
 if (pString == "Likelihood")        { _problem = new Korali::Problem::Likelihood(); foundProblem = true; }
 if (pString == "Posterior")         { _problem = new Korali::Problem::Posterior();  foundProblem = true; }
 if (foundProblem == false) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Problem."); exit(-1); }
 _problem->setConfiguration(js["Problem"]);
 js.erase("Problem");

 // Configure Conduit
 std::string conduitString = "Sequential";

 #ifdef _KORALI_USE_UPCXX
 upcxx::init();
 if (upcxx::rank_n() > 1) conduitString = "UPC++";
 #endif

 conduitString = consume<std::string>(js, { "Conduit", "Type" }, conduitString);
 _conduit = NULL;

 if (conduitString == "Sequential")
  _conduit = new Korali::Conduit::Single();

 if (conduitString == "UPC++")
 {
  #ifdef _KORALI_USE_UPCXX
   _conduit = new Korali::Conduit::UPCXX();
  #else
   fprintf(stderr, "[Korali] Error: UPC++ conduit is not properly configured.\n");
   fprintf(stderr, "[Korali] Reinstall Korali with the proper configuration to support UPC++.\n");
   exit(-1);
  #endif
 }

 if (conduitString == "OpenMP")
 {
  #ifdef _KORALI_USE_OPENMP
   _conduit = new Korali::Conduit::OpenMP();
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

 if (js.find("Conduit") != js.end())
 {
  _conduit->setConfiguration(js["Conduit"]);
  js.erase("Conduit");
 }

 // Configure Solver
 _solver = NULL;
 auto sString = consume<std::string>(js, { "Solver", "Method" });
 if (sString == "CMA-ES") _solver = new Korali::Solver::CMAES();
 if (sString == "TMCMC")  _solver = new Korali::Solver::TMCMC();
 if (_solver == NULL) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Solver."); exit(-1); }
 _solver->setConfiguration(js["Solver"]);
 js.erase("Solver");

 if (js.size() > 0)
 {
  fprintf(stderr, "[Korali] Error: Unrecognized Settings for Korali:\n");
  fprintf(stderr, "%s\n", js.dump(2).c_str());
  exit(-1);
 }
}

