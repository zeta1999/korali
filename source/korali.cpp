#include "korali.h"
#include <chrono>

Korali::Engine* Korali::_k;

Korali::Engine::Engine()
{
 // Setting Defaults
 N = 0;
 _reportFrequency = 1;
 _statisticalParameterCount = 0;
 _computationalParameterCount = 0;
 _verbosity = KORALI_NORMAL;
 std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::nanoseconds(0));
 _seed  = std::chrono::nanoseconds(now_c).count();
 gsl_rng_env_setup();
}

void Korali::Engine::initialize()
{
 // Configure Korali Engine
 if (_config.find("Seed") != _config.end()) if (_config["Seed"].is_number())
 { _seed = _config["Seed"]; _config.erase("Seed"); }

 auto vString = consumeString(_config, { "Verbosity" });
 if (vString == "Silent")   _verbosity = KORALI_SILENT;
 if (vString == "Minimal")  _verbosity = KORALI_MINIMAL;
 if (vString == "Normal")   _verbosity = KORALI_NORMAL;
 if (vString == "Detailed") _verbosity = KORALI_DETAILED;

 if (_config.find("Report Frequency") != _config.end()) if (_config["Report Frequency"].is_number())
 { _reportFrequency = _config["Report Frequency"]; _config.erase("Report Frequency"); }

 // Configure Parameters
 std::vector<Korali::Parameter::Base*> tmp;

 if (isArray(_config, { "Parameters" } ))
 for (int i = 0; i < _config["Parameters"].size(); i++)
 {
  auto dString = consumeString(_config["Parameters"][i], { "Distribution" });
  bool foundDistribution = false;
  if (dString == "Uniform")     { tmp.push_back(new Korali::Parameter::Uniform());     foundDistribution = true; }
  if (dString == "Gaussian")    { tmp.push_back(new Korali::Parameter::Gaussian());    foundDistribution = true; }
  if (dString == "Gamma")       { tmp.push_back(new Korali::Parameter::Gamma());       foundDistribution = true; }
  if (dString == "Exponential") { tmp.push_back(new Korali::Parameter::Exponential()); foundDistribution = true; }
  if (foundDistribution == false) { fprintf(stderr, "[Korali] Error: Incorrect or missing distribution for parameter %d.\n", i); exit(-1); }
  tmp[i]->setConfiguration(_config["Parameters"][i]);
 }

 if (tmp.size() == 0) { fprintf(stderr, "[Korali] Error: Incorrect or undefined parameters.\n"); exit(-1); }
 _config.erase("Parameters");

 for (int i = 0; i < tmp.size(); i++) if (tmp[i]->_type == KORALI_COMPUTATIONAL) { _parameters.push_back(tmp[i]); _computationalParameterCount++; }
 for (int i = 0; i < tmp.size(); i++) if (tmp[i]->_type == KORALI_STATISTICAL)   { _parameters.push_back(tmp[i]); _statisticalParameterCount++; };
 N = _parameters.size();

  // Configure Problem
 bool foundProblem = false;
 auto pString =  consumeString(_config, { "Problem", "Objective" });
 if (pString == "Direct Evaluation") { _problem = new Korali::Problem::Direct();     foundProblem = true; }
 if (pString == "Likelihood")        { _problem = new Korali::Problem::Likelihood(); foundProblem = true; }
 if (pString == "Posterior")         { _problem = new Korali::Problem::Posterior();  foundProblem = true; }
 if (foundProblem == false) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Problem."); exit(-1); }
 _problem->setConfiguration(_config["Problem"]);
 _config.erase("Problem");

 // Configure Conduit
 std::string conduitString = "Sequential";

 #ifdef _KORALI_USE_UPCXX
 upcxx::init();
 if (upcxx::rank_n() > 1) conduitString = "UPC++";
 #endif

 auto cString =  consumeString(_config, { "Conduit", "Type" });
 if (cString != "") conduitString = cString;

 bool recognizedConduit = false;

 if (conduitString == "Sequential")
 {
  _conduit = new Korali::Conduit::Single();
  recognizedConduit = true;
 }

 if (conduitString == "UPC++")
 {
  #ifdef _KORALI_USE_UPCXX
   _conduit = new Korali::Conduit::UPCXX();  recognizedConduit = true;
  #else
   fprintf(stderr, "[Korali] Error: UPC++ conduit is not properly configured.\n");
   fprintf(stderr, "[Korali] Reinstall Korali with the proper configuration to support UPC++.\n");
   exit(-1);
  #endif
 }

 if (conduitString == "OpenMP")
 {
  #ifdef _KORALI_USE_OPENMP
   _conduit = new Korali::Conduit::OpenMP(); recognizedConduit = true;
  #else
   fprintf(stderr, "[Korali] Error: OpenMP conduit is not properly configured.\n");
   fprintf(stderr, "[Korali] Reinstall Korali with the proper configuration to support openMP.\n");
   exit(-1);
  #endif
 }

 if (recognizedConduit == false)
 {
   fprintf(stderr, "[Korali] Error: Unrecognized or no conduit ('%s') selected.\n", conduitString.c_str());
   exit(-1);
 }

 if (_config.find("Conduit") != _config.end())
 {
  _conduit->setConfiguration(_config["Conduit"]);
  _config.erase("Conduit");
 }

 // Configure Solver
 bool foundSolver = false;
 auto sString = consumeString(_config, { "Solver", "Method" });
 if (sString == "CMA-ES") { _solver = new Korali::Solver::CMAES(); foundSolver = true; }
 if (sString == "TMCMC")  { _solver = new Korali::Solver::TMCMC(); foundSolver = true; }
 if (foundSolver == false) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Solver."); exit(-1); }
 _solver->setConfiguration(_config["Solver"]);
 _config.erase("Solver");

 if (_config.size() > 0)
 {
  fprintf(stderr, "[Korali] Error: Unrecognized Settings for Korali:\n");
  fprintf(stderr, "%s\n", _config.dump(2).c_str());
  exit(-1);
 }

 // Initializing Modules
 _problem->initialize();
 for(int i = 0; i < _parameters.size(); i++)  _parameters[i]->initialize(_seed++);
 _conduit->initialize();
 _solver->initialize();
}

void Korali::Engine::run()
{
 _k = this;

 initialize();

 _conduit->run();

 #ifdef _KORALI_USE_UPCXX
 upcxx::finalize();
 #endif

 return;
}
