#ifndef _CONDUIT_H_
#define _CONDUIT_H_

#include "base.h"
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

  Conduit(BaseProblem* problem);
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
	void run();
	void supervisorThread();
	void workerThread();
	virtual void runSolver() = 0;                                      // To be satisfied by the sampler/optimizer
	virtual void processSample(size_t sampleId, double fitness) = 0;   // To be satisfied by the sampler/optimizer
};

} // namespace Korali

#endif // _CONDUIT_H_
