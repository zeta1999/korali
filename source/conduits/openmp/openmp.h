#ifdef _KORALI_USE_OPENMP

#ifndef _KORALI_OPENMP_H_
#define _KORALI_OPENMP_H_

#include "conduits/base/base.h"
#include "pthread.h"
#include <queue>

namespace Korali::Conduit
{

class Lock
{
 pthread_mutex_t _lock;
 public:
 Lock() { pthread_mutex_init(&_lock, 0); }
 ~Lock() { pthread_mutex_destroy(&_lock); }
 void lock() { pthread_mutex_lock(&_lock); }
 void unlock() { pthread_mutex_unlock(&_lock); }
 bool trylock() { return pthread_mutex_trylock(&_lock) == 0; }
};

class OpenMP : public Base
{
 public:
 int _threadCount;
 bool _continueEvaluations;

 std::queue<size_t> _sampleQueue;
 double* sampleArrayPointer;
 double* fitnessArrayPointer;
 Lock _queueLock;

 void run();
 void evaluateSample(double* sampleArray, size_t sampleId);
 void checkProgress();
 void supervisorThread();
 void workerThread();
 bool isRoot();

 // Constructor / Destructor
 OpenMP(nlohmann::json& js);
 ~OpenMP();

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_OPENMP_H_

#endif // _KORALI_USE_OPENMP
