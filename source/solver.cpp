#include "solver.h"

Korali::Solver* _k;

Korali::Solver::Solver(Problem* problem, MPI_Comm comm)
{
  _problem = problem;
	_comm = comm;

	_rankId = 0;
	_rankCount = 1;
  _continueEvaluations = true;

  _sampleSize = 1000;
}

void Korali::Solver::run()
{
	_k = this;
	upcxx::init();
	_rankId = upcxx::rank_me();
	_rankCount = upcxx::rank_n();

  if (_rankId == 0) supervisorThread(); else workerThread();

  upcxx::barrier();
  upcxx::finalize();
}
