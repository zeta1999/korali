#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "prior.h"

namespace Korali
{

class Problem
{
  public:

	Problem(size_t dim, double (*fun) (double*, int), size_t seed);

  Prior* getPrior(int prior) { return &_priors[prior]; }
  Prior* operator[](int prior) { return getPrior(prior); }

  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);

	size_t _dimCount;
	size_t _seed;
  double (*_fitnessFunction) (double*, int);
  Prior* _priors;
};

} // namespace Korali


#endif // _PROBLEM_H_
