#ifndef _KORALI_DIRECT_H_
#define _KORALI_DIRECT_H_

#include "problems/base.h"

namespace Korali::Problem
{

class Direct : public Korali::Problem::Base
{
 public:
 double (*_modelFunction) (double*);
 Direct(double (*modelFunction) (double*));
 double evaluateFitness(double* sample);

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);
};

} // namespace Korali


#endif // _KORALI_DIRECT_H_
