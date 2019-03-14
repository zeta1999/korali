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

  size_t _sampleSize;

  Problem* _problem;
  bool _continueEvaluations;

  Solver(Problem* problem, MPI_Comm comm);
	void setPopulationSize(int size) { _sampleSize = size; }
	void run();

	virtual void supervisorThread() = 0;
	virtual void workerThread() = 0;
};

} // namespace Korali

extern Korali::Solver* _k;

#endif // _SOLVER_H_
