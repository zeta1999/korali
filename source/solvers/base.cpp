#include "solvers/base.h"
#include "conduits/base.h"

Korali::BaseSolver::BaseSolver(BaseProblem* problem)
{
  _problem = problem;
  _verbose = false;
  _sampleCount = 1000;
	_maxGens = 200;

	N = _problem->_parameterCount;
}

void Korali::BaseSolver::run()
{
  _problem->initializeParameters();
	_conduit = new Conduit(this);
  _conduit->initialize();
}
