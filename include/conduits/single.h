#ifndef _CONDUIT_H_
#define _CONDUIT_H_

#include "conduits/base.h"

namespace Korali
{

class Conduit : public BaseConduit {
  public:

	double* sampleArrayPointer;

  Conduit(BaseSolver* solver);
  void initialize();
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
};

} // namespace Korali

#endif // _CONDUIT_H_
