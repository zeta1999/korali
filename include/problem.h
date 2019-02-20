#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "parameter.h"
#include "cmaes.h"

namespace Korali
{

class Problem
{
  public:

	Problem(double (*fun) (double*, int, void*), size_t seed = 0);

	void addParameter(Parameter p);
  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);
  double evaluateFitness(double* sample);
  void setReferenceData(void* refData);
  KoraliCMAES* _engine;

  void solve() { _engine->run(); }
  double (*_fitnessFunction) (double*, int, void*);
  size_t _dimCount;
	size_t _seed;
	void* _refDataBuffer;
  std::vector<Parameter> _parameters;
};

} // namespace Korali


#endif // _PROBLEM_H_
