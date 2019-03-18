#ifndef _ENGINE_H_
#define _ENGINE_H_

#include "base.h"

namespace Korali
{

class Engine : public BaseEngine {
  public:

	double* sampleArrayPointer;

  Engine(Problem* problem);
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
	void run();

	virtual void runEngine() = 0;
	virtual void processSample(size_t sampleId, double fitness) = 0;
};

} // namespace Korali

#endif // _ENGINE_H_
