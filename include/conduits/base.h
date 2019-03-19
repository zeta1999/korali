#ifndef _BASECONDUIT_H_
#define _BASECONDUIT_H_

#include <stdlib.h>
#include "problem.h"

namespace Korali
{

class BaseConduit {
  public:

	// Korali Runtime Variables
  size_t _maxGens;                  // Max number of Conduit Generations
  size_t _sampleCount;
	size_t  N; // Parameter Count
  Problem* _problem;
  bool _verbose;

  BaseConduit(Problem* problem);
	void setPopulationSize(int size) { _sampleCount = size; }
	void setVerbose(bool verbose) { _verbose = verbose; }
	void setMaxGenerations(int maxGens) { _maxGens = maxGens; }

	virtual void evaluateSample(size_t sampleId) = 0; // To be satisfied by the conduit
	virtual double* getSampleArrayPointer()  = 0;     // To be satisfied by the conduit
	virtual void checkProgress() = 0;                 // To be satisfied by the conduit
	virtual void run();                               // To be satisfied by the conduit

	virtual void runSolver() = 0;                                    // To be satisfied by the sampler/optimizer
	virtual void processSample(size_t sampleId, double fitness) = 0; // To be satisfied by the sampler/optimizer
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
	Conduit(Problem* problem) : BaseConduit(problem) {}
	void run()
	{
		fprintf(stderr, "[Korali] Error: No Korali communication conduit was set.\n");
		fprintf(stderr, "[Korali] Use: $export KORALI_CONDUIT={mpi, upcxx, single}, and recompile.\n");
		exit(-1);
	}
	double* getSampleArrayPointer(){return 0;}
	void evaluateSample(size_t sampleId){}
	void checkProgress(){}
	};
}
#endif


#endif // _BASECONDUIT_H_
