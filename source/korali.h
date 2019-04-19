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
#include "pybind11/functional.h"
#include "pybind11/stl.h"

enum verbosity { KORALI_SILENT = 0, KORALI_MINIMAL = 1, KORALI_NORMAL = 2, KORALI_DETAILED = 3 };

namespace Korali
{

class modelData
{
 public:
	double getParameter(size_t i)
	{
		if (i > _parameters.size())
		{
		   fprintf(stderr, "[Korali] Error: Trying to access parameter %lu, when only %lu are provided.\n", i, _parameters.size());
		   exit(-1);
	  }
		return _parameters[i];
	}

	void addResult(double x) {_result.push_back(x); }

  std::vector<double> _parameters;
  std::vector<double> _result;
};

class KoraliJsonWrapper
{
 public:
  nlohmann::json* _js;

  KoraliJsonWrapper& getItem(const std::string& key)           { _js = &((*_js)[key]); return *this;}
  KoraliJsonWrapper& getItem(const unsigned long int& key)     { _js = &((*_js)[key]); return *this;}
  void setItem(const std::string& key, const std::string& val) { (*_js)[key] = val; }
  void setItem(const std::string& key, const double& val)      { (*_js)[key] = val; }
  void setItem(const std::string& key, const int& val)         { (*_js)[key] = val; }
  void setItem(const std::string& key, const bool& val)        { (*_js)[key] = val; }
  void setItem(const int& key, const std::string& val)         { (*_js)[key] = val; }
  void setItem(const int& key, const double& val)              { (*_js)[key] = val; }
  void setItem(const int& key, const int& val)                 { (*_js)[key] = val; }
  void setItem(const int& key, const bool& val)                { (*_js)[key] = val; }
};

class Engine {

 public:

 nlohmann::json  _js;
 KoraliJsonWrapper _wr;

 nlohmann::json& operator[](std::string key) { return _js[key]; }

 std::function<modelData(modelData&)> _model;
 Korali::Conduit::Base* _conduit;
 Korali::Problem::Base* _problem;
 Korali::Solver::Base*  _solver;
 std::vector<Korali::Parameter::Base*> _parameters;

 // Model Functions and constructors
 Engine();
 Engine(std::function<modelData(modelData&)> model) : Engine::Engine() { _model = model; }
 ~Engine();

 void run();
 KoraliJsonWrapper& getItem(const std::string& key)           { _wr._js = &(_js[key]); return _wr;}
 KoraliJsonWrapper& getItem(const unsigned long int& key)     { _wr._js = &(_js[key]); return _wr;}
 void setItem(const std::string& key, const std::string& val) { _js[key] = val; }
 void setItem(const std::string& key, const double& val)      { _js[key] = val; }
 void setItem(const std::string& key, const int& val)         { _js[key] = val; }
 void setItem(const std::string& key, const bool& val)        { _js[key] = val; }

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
