#ifndef _BASECONDUIT_H_
#define _BASECONDUIT_H_

#include <stdlib.h>
#include "problems/base.h"

namespace Korali::Solver {
    class Base;
}

namespace Korali::Conduit
{

class Base {
  public:

	Korali::Solver::Base* _solver;

  Base(Korali::Solver::Base* solver);
  virtual void initialize() = 0;
	virtual void evaluateSample(size_t sampleId) = 0;
	virtual double* getSampleArrayPointer()  = 0;
	virtual void checkProgress() = 0;
};

class Conduit;

} // namespace Korali

// Choosing correct conduit from compilation defined values

#ifdef KORALI_UPCXX_CONDUIT
#include "upcxx.h"
#endif

#ifdef KORALI_MPI_CONDUIT
#include "mpi.h"
#endif

#ifdef KORALI_SINGLE_CONDUIT
#include "single.h"
#endif


#endif // _BASECONDUIT_H_
