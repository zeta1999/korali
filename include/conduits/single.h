#ifndef _KORALI_SINGLE_H_
#define _KORALI_SINGLE_H_

#include "conduits/base.h"

namespace Korali::Conduit
{

class Single : public Base {
  public:

	double* sampleArrayPointer;

	Single(Korali::Solver::Base* solver);
  void initialize();
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
};

} // namespace Korali

#endif // _KORALI_SINGLE_H_
