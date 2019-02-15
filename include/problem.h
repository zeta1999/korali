#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "prior.h"

namespace Korali
{

class Problem
{
  public:

	Problem(std::string type, size_t dim, double (*fun) (double*, int), size_t seed);

  Prior* getPrior(int prior) { return &_priors[prior]; }
  Prior* operator[](int prior) { return getPrior(prior); }

  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);
  double evaluateFitness(double* sample);

	size_t _dimCount;
	size_t _seed;
	std::string _type;
  double (*_fitnessFunction) (double*, int);
  Prior* _priors;
};


} // namespace Korali


#endif // _PROBLEM_H_
