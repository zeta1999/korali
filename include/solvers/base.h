#ifndef _BASESOLVER_H_
#define _BASESOLVER_H_

#include <stdlib.h>
#include "problems/base.h"

namespace Korali
{

class Conduit;

class BaseSolver {
  public:

	// Korali Runtime Variables
  size_t _maxGens;                  // Max number of Conduit Generations
  size_t _sampleCount;
	size_t  N; // Parameter Count
  BaseProblem* _problem;
  bool _verbose;
  Conduit* _conduit;

  BaseSolver(BaseProblem* problem);
	void setPopulationSize(int size) { _sampleCount = size; }
	void setVerbose(bool verbose) { _verbose = verbose; }
	void setMaxGenerations(int maxGens) { _maxGens = maxGens; }

	virtual void run();                                    // To be satisfied by the sampler/optimizer
	virtual void runSolver() = 0;                                    // To be satisfied by the sampler/optimizer
	virtual void processSample(size_t sampleId, double fitness) = 0; // To be satisfied by the sampler/optimizer
};

} // namespace Korali

#endif // _BASESOLVER_H_
