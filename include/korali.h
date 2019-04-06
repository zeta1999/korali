#ifndef _KORALI_H_
#define _KORALI_H_

#include "json.hpp"
#include "stdlib.h"
#include <vector>
#include <functional>

#include "problems/direct.h"
#include "problems/likelihood.h"
#include "problems/posterior.h"

#include "parameters/exponential.h"
#include "parameters/gamma.h"
#include "parameters/gaussian.h"
#include "parameters/uniform.h"

#include "solvers/cmaes.h"
#include "solvers/tmcmc.h"

#include "conduits/single.h"
#include "conduits/upcxx.h"

#include "json.hpp"

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
  Engine(std::function<void (double*, double*, double*, double*)> model) : Engine::Engine() {_modelManifold = model;	_config["Problem"]["Model"] = "Manifold"; }

  void run();

  // Parameter Management
  bool isSampleOutsideBounds(double* sample);
  double getPriorsLogProbabilityDensity(double *x);
  double getPriorsProbabilityDensity(double *x);
  void initialize();

  size_t S; // Sample Size
  size_t N; // Parameter Count
  size_t _seed;
  int _verbosity;
  size_t _reportFrequency;

  // Serialization Methods
  virtual nlohmann::json getConfiguration();
  virtual void setConfiguration(nlohmann::json js);
};

extern Engine* _k;

} // namespace Korali

#endif // _KORALI_H_
