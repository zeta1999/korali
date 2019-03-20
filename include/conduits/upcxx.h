#ifndef _KORALI_UPCXX_H_
#define _KORALI_UPCXX_H_
#ifdef UPCXX_VERSION

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

  UPCXX(Korali::Solver::Base* solver);
  void initialize();
	void evaluateSample(size_t sampleId);
	double* getSampleArrayPointer();
	void checkProgress();
	void supervisorThread();
	void workerThread();
};

} // namespace Korali

extern Korali::Conduit::UPCXX* _k;

#endif // UPCXX_VERSION
#endif // _KORALI_UPCXX_H_
