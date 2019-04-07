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

#include "solvers/cmaes/cmaes.h"
#include "solvers/tmcmc/tmcmc.h"

#include "conduits/single/single.h"
#include "conduits/upcxx/upcxx.h"

#include "json/json.hpp"

enum verbosity { KORALI_SILENT = 0, KORALI_MINIMAL = 1, KORALI_NORMAL = 2, KORALI_DETAILED = 3 };

namespace Korali
{

class Engine {

 public:

	nlohmann::json _config;

	nlohmann::json& operator [](std::string key) { return _config[key]; }

  Korali::Conduit::Base* _conduit;
  Korali::Problem::Base* _problem;
  Korali::Solver::Base*  _solver;
  std::vector<Korali::Parameter::Base*> _parameters;

  // Model Functions and constructors
  Engine();

  std::function<double (double*)> _modelSingle;
  Engine(std::function<double (double*)> model) : Engine::Engine() { _modelSingle = model; _config["Problem"]["Model"] = "Single"; }

  std::function<void (double*, double*)> _modelMultiple;
  Engine(std::function<void (double*, double*)> model) : Engine::Engine() {	_modelMultiple = model;	_config["Problem"]["Model"] = "Multiple"; }

  std::function<void (double*, double*, double*, double*)> _modelManifold;
  Engine(std::function<void (double*, double*, double*, double*)> model) : Engine::Engine() { _modelManifold = model;	_config["Problem"]["Model"] = "Manifold"; }

  void run();
  void initialize();

  size_t N; // Parameter Count
  size_t _seed;
  int _verbosity;
  size_t _reportFrequency;

  // Serialization Methods
  nlohmann::json getConfiguration();
  void setConfiguration(nlohmann::json js);
};

extern Engine* _k;

} // namespace Korali

#endif // _KORALI_H_
