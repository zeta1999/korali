#include "korali.h"
#include <chrono>

using json = nlohmann::json;

Korali::Engine* Korali::_k;

Korali::Engine::Engine()
{
 // Setting Defaults
 N = 0;
 _reportFrequency = 1;
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

 if (_config.find("Verbosity") != _config.end()) if (_config["Verbosity"].is_string())
 {
	 bool found = false;
	 if (_config["Verbosity"] == "Silent") { _verbosity = KORALI_SILENT; found = true; }
	 if (_config["Verbosity"] == "Minimal") { _verbosity = KORALI_MINIMAL; found = true; }
	 if (_config["Verbosity"] == "Normal")  { _verbosity = KORALI_NORMAL; found = true; }
	 if (_config["Verbosity"] == "Detailed") { _verbosity = KORALI_DETAILED; found = true; }
	 if (found) _config.erase("Verbosity");
 }

 if (_config.find("Report Frequency") != _config.end()) if (_config["Report Frequency"].is_number())
 { _reportFrequency = _config["Report Frequency"]; _config.erase("Report Frequency"); }

 // Configure Parameters
 if (_config.find("Parameters") != _config.end()) if(_config["Parameters"].is_array())
 if (_config["Parameters"].size() > 0) for (int i = 0; i < _config["Parameters"].size(); i++)
 {
	 if (_config["Parameters"][i].find("Distribution") != _config["Parameters"][i].end())
	 if (_config["Parameters"][i]["Distribution"].find("Type") != _config["Parameters"][i]["Distribution"].end())
	 if (_config["Parameters"][i]["Distribution"]["Type"].is_string())
	 {
		bool foundDistribution = false;
	  if (_config["Parameters"][i]["Distribution"]["Type"] == "Uniform")     { _parameters.push_back(new Korali::Parameter::Uniform());     foundDistribution = true; }
	  if (_config["Parameters"][i]["Distribution"]["Type"] == "Gaussian")    { _parameters.push_back(new Korali::Parameter::Gaussian());    foundDistribution = true; }
	  if (_config["Parameters"][i]["Distribution"]["Type"] == "Gamma")       { _parameters.push_back(new Korali::Parameter::Gamma());       foundDistribution = true; }
	  if (_config["Parameters"][i]["Distribution"]["Type"] == "Exponential") { _parameters.push_back(new Korali::Parameter::Exponential()); foundDistribution = true; }
	  if (foundDistribution == false) { fprintf(stderr, "[Korali] Error: Incorrect or parameter distribution: \n %s.\n", _config["Parameters"][i]["Distribution"]["Type"].dump(1).c_str() ); exit(-1); }
	  _parameters[i]->setConfiguration(_config["Parameters"][i]);
	 }
 }
 if (_parameters.size() == 0) { fprintf(stderr, "[Korali] Error: Incorrect or undefined parameters.\n"); exit(-1); }
 _config.erase("Parameters");
 N = _parameters.size();

  // Configure Problem
 bool foundProblem = false;
 if (_config.find("Problem") != _config.end())
 if (_config["Problem"].find("Objective") != _config["Problem"].end()) if(_config["Problem"]["Objective"].is_string())
 {
	 if (_config["Problem"]["Objective"] == "Direct Evaluation") { _problem = new Korali::Problem::Direct();     foundProblem = true; }
	 if (_config["Problem"]["Objective"] == "Likelihood")        { _problem = new Korali::Problem::Likelihood(); foundProblem = true; }
	 if (_config["Problem"]["Objective"] == "Posterior")         { _problem = new Korali::Problem::Posterior();  foundProblem = true; }
 }
 if (foundProblem == false) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Problem."); exit(-1); }
 _problem->setConfiguration(_config["Problem"]);
 _config.erase("Problem");

 // Configure Conduit

 std::string conduitString = "Sequential";

 #ifdef _KORALI_USE_UPCXX
 upcxx::init();
 if (upcxx::rank_n() > 1) conduitString = "UPC++";
 #endif

 if (_config.find("Conduit") != _config.end())
 if (_config["Conduit"].find("Type") != _config["Conduit"].end()) if(_config["Conduit"]["Type"].is_string()) conduitString = _config["Conduit"]["Type"];

 bool recognizedConduit = false;

 if (conduitString == "Sequential") { _conduit = new Korali::Conduit::Single(); recognizedConduit = true; }

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
   fprintf(stderr, "[Korali] Error: Unrecognized conduit '%s' selected.\n", conduitString.c_str());
   exit(-1);
 }

 if (_config.find("Conduit") != _config.end())
 {
  _conduit->setConfiguration(_config["Conduit"]);
  _config.erase("Conduit");
 }

 // Configure Solver
 bool foundSolver = false;
 if (_config.find("Solver") != _config.end())
 if (_config["Solver"].find("Method") != _config["Solver"].end()) if(_config["Solver"]["Method"].is_string())
 {
	 if (_config["Solver"]["Method"] == "CMA-ES") { _solver = new Korali::Solver::CMAES(); foundSolver = true; }
	 if (_config["Solver"]["Method"] == "TMCMC")  { _solver = new Korali::Solver::TMCMC(); foundSolver = true; }
 }
 if (foundSolver == false) { fprintf(stderr, "[Korali] Error: Incorrect or undefined Solver."); exit(-1); }
 _solver->setConfiguration(_config["Solver"]);
 _config.erase("Solver");

 if (_config.size() > 0)
 {
	 fprintf(stderr, "[Korali] Warning: Unrecognized Settings for Korali:\n");
	 fprintf(stderr, "%s\n", _config.dump(2).c_str());
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
