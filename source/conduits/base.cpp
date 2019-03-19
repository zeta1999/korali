#include "conduits/base.h"

Korali::Conduit* _k;

Korali::BaseConduit::BaseConduit(BaseProblem* problem)
{
  _problem = problem;
  _verbose = false;
  _sampleCount = 1000;
	_maxGens = 200;

	N = _problem->_parameterCount;
}

void Korali::BaseConduit::run()
{
  _problem->initializeParameters();
}
