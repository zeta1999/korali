#include "engines/base.h"

Korali::Engine* _k;

Korali::BaseEngine::BaseEngine(Problem* problem)
{
  _problem = problem;
  _verbose = false;
  _sampleCount = 1000;
	_maxGens = 200;

	N = _problem->_parameterCount;
}

void Korali::BaseEngine::run()
{
  _problem->initializeParameters();
}
