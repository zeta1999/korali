#ifdef _KORALI_USE_UPCXX

#ifndef _KORALI_UPCXX_H_
#define _KORALI_UPCXX_H_

#include "conduits/base/base.h"
#include <upcxx/upcxx.hpp>
#include <queue>

namespace Korali::Conduit
{

class UPCXX : public Base
{
 public:
 int _rankId;
 int _rankCount;

 size_t _sampleId;
 bool _continueEvaluations;
 bool _evaluateSample;

 std::queue<int> _workers;
 upcxx::global_ptr<double> sampleGlobalPtr; // Global Pointer for Sample parameters

 UPCXX();
 void initialize();
 void run();
 void evaluateSample(size_t sampleId);
 double* getSampleArrayPointer();
 void checkProgress();
 void supervisorThread();
 void workerThread();

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_UPCXX_H_

#endif // _KORALI_USE_UPCXX
