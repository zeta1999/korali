#ifndef _KORALI_POSTERIOR_H
#define _KORALI_POSTERIOR_H

#include "problems/likelihood.h"

namespace Korali
{

class Posterior : public Likelihood
{
  public:

	Posterior(double* (*modelFunction) (double*), size_t seed = -1);
	double evaluateFitness(double* sample);
};

} // namespace Korali


#endif // _KORALI_POSTERIOR_H
