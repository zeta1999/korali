#ifndef _KORALI_OPENMP_H_
#define _KORALI_OPENMP_H_

#include "conduits/base.h"
#include <queue>

namespace Korali::Conduit
{

class OpenMP : public Base
{
  public:
  int _threadCount;
  bool _continueEvaluations;

	std::queue<int> _workers;
	bool* _evaluateSampleFlag;
	size_t* _evaluateSampleId;
	double* sampleArrayPointer;

	OpenMP(Korali::Solver::Base* solver);
  void initialize();
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
	void supervisorThread();
	void workerThread();
};

} // namespace Korali

#endif // _KORALI_OPENMP_H_
