#ifndef _BASEPROBLEM_H_
#define _BASEPROBLEM_H_

#include "parameter.h"

namespace Korali
{

class BaseProblem
{
  public:

	BaseProblem(size_t seed = 0);

	void addParameter(Parameter p);
  virtual double evaluateFitness(double* sample) = 0;

  size_t _parameterCount;
	size_t _seed;

  std::vector<Parameter> _parameters;

  double getPriorsLogProbabilityDensity(double *x);
  double getPriorsProbabilityDensity(double *x);
  void initializeParameters();
};

} // namespace Korali


#endif // _BASEPROBLEM_H_
