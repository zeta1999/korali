#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "parameter.h"
#include "cmaes.h"

namespace Korali
{

class ProblemBase
{
  public:

	ProblemBase(void (*fun) (double*, int, double*), size_t seed = 0);

	void addParameter(Parameter p);
  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);
  virtual double evaluateFitness(double* sample) = 0;

  void (*_fitnessFunction) (double*, int, double*);
  size_t _dimCount;
	size_t _seed;
  std::vector<Parameter> _parameters;
};

class Maximizer : public ProblemBase
{
  public:

	KoraliCMAES* _engine;
	KoraliCMAES* getEngine() { return _engine; }
	void solve() { _engine->run(); }
	Maximizer(void (*fun) (double*, int, double*), size_t seed = 0);
	double evaluateFitness(double* sample);
};

class Minimizer : public ProblemBase
{
  public:

	KoraliCMAES* _engine;
	KoraliCMAES* getEngine() { return _engine; }
	void solve() { _engine->run(); }
	Minimizer(void (*fun) (double*, int, double*), size_t seed = 0);
	double evaluateFitness(double* sample);
};

class Posterior : public ProblemBase
{
  public:

	KoraliCMAES* _engine;
	KoraliCMAES* getEngine() { return _engine; }
	void solve() { _engine->run(); }
	Posterior(void (*fun) (double*, int, double*), size_t seed = 0);
	void addReferenceData(double ref);
  std::vector<double> _refData;
	double evaluateFitness(double* sample);
};

} // namespace Korali


#endif // _PROBLEM_H_
