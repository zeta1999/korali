#ifndef _KORALI_SINGLE_H_
#define _KORALI_SINGLE_H_

#include "conduits/base.h"

namespace Korali::Conduit
{

class Single : public Base {
 public:

 double* sampleArrayPointer;
 double* fitnessArrayPointer;

 Single();
 void initialize();
 void run();
 void evaluateSample(size_t sampleId);
 double* getSampleArrayPointer();
 void checkProgress();

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_SINGLE_H_
