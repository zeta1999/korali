#ifndef _KORALI_SINGLE_H_
#define _KORALI_SINGLE_H_

#include "conduits/base/base.h"

namespace Korali::Conduit
{

class Single : public Base {
 public:

 void run();
 void evaluateSample(double* sampleArray, size_t sampleId);
 void checkProgress();

 // Constructor / Destructor
 Single(nlohmann::json& js);
 ~Single();

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_SINGLE_H_
