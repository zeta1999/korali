#ifndef _BASECONDUIT_H_
#define _BASECONDUIT_H_

#include <stdlib.h>
#include "problems/base.h"

namespace Korali
{

class BaseSolver;

class BaseConduit {
  public:

  BaseSolver* _solver;

  BaseConduit(BaseSolver* solver);
  virtual void initialize() = 0;
	virtual void evaluateSample(size_t sampleId) = 0; // To be satisfied by the conduit
	virtual double* getSampleArrayPointer()  = 0;     // To be satisfied by the conduit
	virtual void checkProgress() = 0;                 // To be satisfied by the conduit
};

class Conduit;

} // namespace Korali

extern Korali::Conduit* _k;

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

// In case no conduit is selected, exit with error
#ifdef KORALI_NO_CONDUIT
namespace Korali
{
class Conduit : public BaseConduit {
	public:
	Conduit(BaseSolver* solver) : BaseConduit(solver)
	{
		fprintf(stderr, "[Korali] Error: No Korali communication conduit was set.\n");
		fprintf(stderr, "[Korali] Use: $export KORALI_CONDUIT={mpi, upcxx, single}, and recompile.\n");
		exit(-1);
	}
	double* getSampleArrayPointer(){return 0;}
	void initialize(){};
	void evaluateSample(size_t sampleId){}
	void checkProgress(){}
	};
}
#endif


#endif // _BASECONDUIT_H_
