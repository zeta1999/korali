#include "solvers/base.h"
#include "conduits/base.h"

Korali::Solver::Base::Base(BaseProblem* problem)
{
  _problem = problem;
  _verbose = false;
  _sampleCount = 1000;
	_maxGens = 200;

	N = _problem->_parameterCount;
}

void Korali::Solver::Base::run()
{
  _problem->initializeParameters();
	_conduit = new Korali::Conduit::UPCXX(this);
  _conduit->initialize();
}
