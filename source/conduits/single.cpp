#ifdef _KORALI_SINGLE_CONDUIT
#include "conduits/single.h"
#include "solvers/base.h"

Korali::Conduit::Conduit(BaseSolver* solver) : Korali::BaseConduit::BaseConduit (solver) {}

void Korali::Conduit::initialize()
{
	_k = this;

  // Allocating Global Pointer for Samples
	sampleArrayPointer  = (double*) calloc (_solver->N*_solver->_sampleCount, sizeof(double));

	_solver->runSolver();
}

double* Korali::Conduit::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::evaluateSample(size_t sampleId)
{
	double fitness = _solver->_problem->evaluateFitness(&sampleArrayPointer[_solver->N*sampleId]);
	_solver->processSample(sampleId, fitness);
}

void Korali::Conduit::checkProgress()
{

}
#endif
