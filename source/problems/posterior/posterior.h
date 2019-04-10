#ifndef _KORALI_POSTERIOR_H
#define _KORALI_POSTERIOR_H

#include "problems/likelihood/likelihood.h"

namespace Korali::Problem
{

class Posterior : public Korali::Problem::Likelihood
{
 public:
 double evaluateFitness(double* sample);
 void initialize();

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json& js);
};

} // namespace Korali


#endif // _KORALI_POSTERIOR_H
