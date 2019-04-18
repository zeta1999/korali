#ifndef _KORALI_H_
#define _KORALI_H_

#include "stdlib.h"
#include <vector>
#include <functional>

#include "problems/direct/direct.h"
#include "problems/likelihood/likelihood.h"
#include "problems/posterior/posterior.h"

#include "parameters/exponential/exponential.h"
#include "parameters/gamma/gamma.h"
#include "parameters/gaussian/gaussian.h"
#include "parameters/uniform/uniform.h"
#include "parameters/cauchy/cauchy.h"
#include "parameters/laplace/laplace.h"

#include "solvers/cmaes/cmaes.h"
#include "solvers/tmcmc/tmcmc.h"

#include "conduits/sequential/sequential.h"
#include "conduits/upcxx/upcxx.h"

#include "koralijson/koralijson.h"
#include "pybind11/pybind11.h"

enum verbosity { KORALI_SILENT = 0, KORALI_MINIMAL = 1, KORALI_NORMAL = 2, KORALI_DETAILED = 3 };

namespace Korali
{

class KoraliJsonWrapper
{
 public:
  nlohmann::json* _js;

  KoraliJsonWrapper& getItem(const std::string& key){printf("Wrapper: getItem: %s\n", key.c_str()); _js = &((*_js)[key]); return *this;}
  KoraliJsonWrapper& getItem(const unsigned long int& key){printf("Wrapper: getItem: %lu\n", key); _js = &((*_js)[key]); return *this;}
  void setItem(const std::string& key, const std::string& val) {printf("WrapperString: setItem: %s = %s\n", key.c_str(), val.c_str()); (*_js)[key] = val; }
  void setItem(const std::string& key, const double& val)      {printf("WrapperDouble: setItem: %s = %f\n", key.c_str(), val); (*_js)[key] = val; }
  void setItem(const std::string& key, const int& val)         {printf("WrapperInt: setItem: %s = %d\n", key.c_str(), val); (*_js)[key] = val; }
  void setItem(const std::string& key, const bool& val)        {printf("WrapperBool: setItem: %s = %d\n", key.c_str(), val); (*_js)[key] = val; }
};

class Engine {

 public:

 nlohmann::json  _js;
 KoraliJsonWrapper _wr;

 nlohmann::json& operator[](std::string key) { return _js[key]; }

 Korali::Conduit::Base* _conduit;
 Korali::Problem::Base* _problem;
 Korali::Solver::Base*  _solver;
 std::vector<Korali::Parameter::Base*> _parameters;

 // Model Functions and constructors
 Engine();

 std::function<double (double*)> _modelSingle;
 Engine(std::function<double (double*)> model) : Engine::Engine() { _modelSingle = model; _js["Problem"]["Model"] = "Single"; }

 std::function<void (double*, double*)> _modelMultiple;
 Engine(std::function<void (double*, double*)> model) : Engine::Engine() { _modelMultiple = model; _js["Problem"]["Model"] = "Multiple"; }

 std::function<void (double*, double*, double*, double*)> _modelManifold;
 Engine(std::function<void (double*, double*, double*, double*)> model) : Engine::Engine() { _modelManifold = model; _js["Problem"]["Model"] = "Manifold"; }

 ~Engine();

 void run();
 KoraliJsonWrapper& getItem(const std::string& key){printf("Engine: GetItem\n"); _wr._js = &(_js[key]); return _wr;}
 KoraliJsonWrapper& getItem(const unsigned long int& key){printf("Engine: GetItem\n"); _wr._js = &(_js[key]); return _wr;}
 void setItem(const std::string& key, const std::string& val) {printf("EngineString: setItem: %s = %s\n", key.c_str(), val.c_str()); _js[key] = val; }
 void setItem(const std::string& key, const double& val)      {printf("EngineDouble: setItem: %s = %f\n", key.c_str(), val); _js[key] = val; }
 void setItem(const std::string& key, const int& val)         {printf("EngineInt: setItem: %s = %d\n", key.c_str(), val); _js[key] = val; }
 void setItem(const std::string& key, const bool& val)        {printf("EngineBool: setItem: %s = %d\n", key.c_str(), val); _js[key] = val; }

 void loadState(std::string fileName);
 void saveState(std::string fileName);
 void saveState();

 size_t N; // Parameter Count
 size_t _statisticalParameterCount;
 size_t _computationalParameterCount;
 size_t _seed;
 int _verbosity;
 size_t _reportFrequency;

 char   _resultsDirName[64];
 size_t _currentState;

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);

};

extern Engine* _k;

} // namespace Korali

#endif // _KORALI_H_
