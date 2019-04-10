#ifndef _KORALI_LIKELIHOOD_H_
#define _KORALI_LIKELIHOOD_H_

#include "problems/base/base.h"

namespace Korali::Problem
{

class Likelihood : public Korali::Problem::Base
{
 public:

 // Reference Data
 double* _referenceData;
 double* fitnessArrayPointer;

 size_t _sigmaPosition;
 size_t _referenceDataSize;
 double evaluateFitness(double* sample);
 void initialize();

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json& js);
};

} // namespace Korali


#endif // _KORALI_LIKELIHOOD_H_
