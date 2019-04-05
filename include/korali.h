#ifndef _KORALI_H_
#define _KORALI_H_

#include "json.hpp"
#include "stdlib.h"
#include <vector>

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

namespace Korali
{

class Engine {

 public:

  Korali::Conduit::Base* _conduit;
  Korali::Problem::Base* _problem;
  Korali::Solver::Base*  _solver;
  std::vector<Korali::Parameter::Base*> _parameters;

  Engine(size_t seed = 0);
  void run();

  // Parameter Management
  void addParameter(Korali::Parameter::Base* p);
  bool isSampleOutsideBounds(double* sample);
  double getPriorsLogProbabilityDensity(double *x);
  double getPriorsProbabilityDensity(double *x);
  void initializeParameters();

  // Reference Data
  double* _referenceData;
  void setReferenceData(size_t nData, double* referenceData);

  size_t _sampleCount;
  size_t _parameterCount;
  size_t _referenceDataSize;
  size_t _seed;
  size_t _maxGens;

  int _verbosity;
  size_t _reportFrequency;
  void setReportVerbosity(int verbosity) { _verbosity = verbosity; }
  void setPopulationSize(int size) { _sampleCount = size; }
  void setMaxGenerations(int maxGens) { _maxGens = maxGens; }

  // Serialization Methods
  virtual nlohmann::json getConfiguration();
  virtual void setConfiguration(nlohmann::json js);
};

extern Engine* _k;

} // namespace Korali

#endif // _KORALI_H_
