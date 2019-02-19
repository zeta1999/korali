#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "parameter.h"

namespace Korali
{

class Problem
{
  public:

	Problem(std::string type, double (*fun) (double*, int), size_t seed = 0);

	void addParameter(Parameter p);
  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);
  double evaluateFitness(double* sample);

  size_t _dimCount;
	size_t _seed;
	std::string _type;
  double (*_fitnessFunction) (double*, int);
  std::vector<Parameter> _parameters;
};


} // namespace Korali


#endif // _PROBLEM_H_
