#ifndef _SOLVER_H_
#define _SOLVER_H_

#include <stdlib.h>
#include "mpi.h"
#include <upcxx/upcxx.hpp>
#include "problem.h"
#include <queue>

namespace Korali
{

class Solver {
  public:

	// Korali Runtime Variables
  MPI_Comm _comm;
  int _rankId;
  int _rankCount;

  size_t _maxGens;                  // Max number of Solver Generations
  size_t _sampleCount;
  size_t _sampleId;
	size_t  N; // Parameter Count
  Problem* _problem;
  bool _continueEvaluations;
	bool   _evaluateSample;
  bool _verbose;

	std::queue<int> _workers;
	upcxx::global_ptr<double> sampleGlobalPtr; // Global Pointer for Sample parameters

  Solver(Problem* problem, MPI_Comm comm);
	void setPopulationSize(int size) { _sampleCount = size; }
	void setVerbose(bool verbose) { _verbose = verbose; }
	void setMaxGenerations(int maxGens) { _maxGens = maxGens; }
	void evaluateSample(size_t sampleId);
	void checkProgress();
	void run();

	void supervisorThread();
	void workerThread();
	virtual void runEngine() = 0;
	virtual void processSample(size_t sampleId, double fitness) = 0;
};

} // namespace Korali

extern Korali::Solver* _k;

#endif // _SOLVER_H_
