#ifndef _KORALI_DIRECT_H_
#define _KORALI_DIRECT_H_

#include "problems/base/base.h"

namespace Korali::Problem
{

class Direct : public Korali::Problem::Base
{
 public:
 Direct();
 double evaluateFitness(double* sample);
 void initialize();

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);
};

} // namespace Korali


#endif // _KORALI_DIRECT_H_
