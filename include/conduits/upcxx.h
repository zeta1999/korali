#ifndef _CONDUIT_H_
#define _CONDUIT_H_

#include "conduits/base.h"
#include <upcxx/upcxx.hpp>
#include <queue>

namespace Korali
{

class Conduit : public BaseConduit
{
  public:
  int _rankId;
  int _rankCount;

  size_t _sampleId;
  bool _continueEvaluations;
	bool _evaluateSample;

	std::queue<int> _workers;
	upcxx::global_ptr<double> sampleGlobalPtr; // Global Pointer for Sample parameters

  Conduit(BaseSolver* solver);
  void initialize();
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
	void supervisorThread();
	void workerThread();
};

} // namespace Korali

#endif // _CONDUIT_H_
