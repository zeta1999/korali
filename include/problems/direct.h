#ifndef _KORALI_DIRECT_H_
#define _KORALI_DIRECT_H_

#include "problems/base.h"

namespace Korali
{

class Direct : public BaseProblem
{
  public:
	double (*_modelFunction) (double*);
	Direct(double (*modelFunction) (double*), size_t seed = -1);
	double evaluateFitness(double* sample);
};

} // namespace Korali


#endif // _KORALI_DIRECT_H_
