#ifdef _KORALI_USE_UPCXX

#ifndef _KORALI_UPCXX_H_
#define _KORALI_UPCXX_H_

#include "conduits/base.h"
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
	double* fitnessArrayPointer;

  UPCXX(Korali::Solver::Base* solver);
  void initialize();
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	double* getFitnessArrayPointer();
	void checkProgress();
	void supervisorThread();
	void workerThread();
};

} // namespace Korali

extern Korali::Conduit::UPCXX* _k;

#endif // _KORALI_UPCXX_H_

#endif // _KORALI_USE_UPCXX
