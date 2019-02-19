#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "parameter.h"

namespace Korali
{

class Problem
{
  public:

	Problem(std::string type, size_t dim, double (*fun) (double*, int), size_t seed);

  Parameter* getParameter(int Parameter) { return &_parameters[Parameter]; }
  Parameter* operator[](int Parameter) { return getParameter(Parameter); }

  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);
  double evaluateFitness(double* sample);

	size_t _dimCount;
	size_t _seed;
	std::string _type;
  double (*_fitnessFunction) (double*, int);
  Parameter* _parameters;
};


} // namespace Korali


#endif // _PROBLEM_H_
