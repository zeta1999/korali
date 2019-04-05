#ifndef _KORALI_LIKELIHOOD_H_
#define _KORALI_LIKELIHOOD_H_

#include "problems/base.h"

namespace Korali::Problem
{

class Likelihood : public Korali::Problem::Base
{
 public:

 void (*_modelFunction) (double*, double*);
 Likelihood(void (*modelFunction) (double*, double*));
 double evaluateFitness(double* sample);

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);
};

} // namespace Korali


#endif // _KORALI_LIKELIHOOD_H_
