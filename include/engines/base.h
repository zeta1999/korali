#ifndef _BASEENGINE_H_
#define _BASEENGINE_H_

#include <stdlib.h>
#include "problem.h"

namespace Korali
{

class BaseEngine {
  public:

	// Korali Runtime Variables
  size_t _maxGens;                  // Max number of Engine Generations
  size_t _sampleCount;
	size_t  N; // Parameter Count
  Problem* _problem;
  bool _verbose;

  BaseEngine(Problem* problem);
	void setPopulationSize(int size) { _sampleCount = size; }
	void setVerbose(bool verbose) { _verbose = verbose; }
	void setMaxGenerations(int maxGens) { _maxGens = maxGens; }

	virtual void evaluateSample(size_t sampleId) = 0; // To be satisfied by the engine
	virtual double* getSampleArrayPointer()  = 0;     // To be satisfied by the engine
	virtual void checkProgress() = 0;                 // To be satisfied by the engine
	virtual void run();                               // To be satisfied by the engine

	virtual void runEngine() = 0;                                    // To be satisfied by the sampler/optimizer
	virtual void processSample(size_t sampleId, double fitness) = 0; // To be satisfied by the sampler/optimizer
};

class Engine;

} // namespace Korali

extern Korali::Engine* _k;


// Choosing correct engine from compilation defined values

#if defined (_KORALI_UPCXX_ENGINE)
#include "upcxx.h"
#elif defined (_KORALI_MPI_ENGINE)
#include "mpi.h"
#else
#include "single.h"
#endif


#endif // _BASEENGINE_H_
