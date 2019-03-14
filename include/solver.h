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

  int _maxGens;                  // Max number of Solver Generations
  size_t _sampleSize;
  size_t _sampleId;
	size_t  N; // Parameter Count
  Problem* _problem;
  bool _continueEvaluations;
	bool   _evaluateSample;

	std::queue<int> _workers;
	upcxx::global_ptr<double> sampleGlobalPtr; // Global Pointer for Sample parameters

  Solver(Problem* problem, MPI_Comm comm);
	void setPopulationSize(int size) { _sampleSize = size; }
	void setMaxGenerations(int _maxGens) { _maxGens = _maxGens; }
	void run();

	void supervisorThread();
	void workerThread();
	virtual void supervisorThread2() = 0;
	virtual void processSample(size_t sampleId, double fitness) = 0;
};

} // namespace Korali

extern Korali::Solver* _k;

#endif // _SOLVER_H_
