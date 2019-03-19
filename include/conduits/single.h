#ifndef _CONDUIT_H_
#define _CONDUIT_H_

#include "base.h"

namespace Korali
{

class Conduit : public BaseConduit {
  public:

	double* sampleArrayPointer;

  Conduit(BaseProblem* problem);
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
	void run();

	virtual void runSolver() = 0;
	virtual void processSample(size_t sampleId, double fitness) = 0;
};

} // namespace Korali

#endif // _CONDUIT_H_
