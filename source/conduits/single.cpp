#ifdef _KORALI_SINGLE_CONDUIT
#include "conduits/single.h"
#include "solvers/base.h"

Korali::Conduit::Single(Korali::Solver::Base* solver) : Korali::Conduit::Base::Base(solver) {}

void Korali::Conduit::Single::initialize()
{
	_k = this;

  // Allocating Global Pointer for Samples
	sampleArrayPointer  = (double*) calloc (_solver->N*_solver->_sampleCount, sizeof(double));

	_solver->runSolver();
}

double* Korali::Conduit::Single::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::Single::evaluateSample(size_t sampleId)
{
	double fitness = _solver->_problem->evaluateFitness(&sampleArrayPointer[_solver->N*sampleId]);
	_solver->processSample(sampleId, fitness);
}

void Korali::Conduit::Single::checkProgress()
{

}
#endif
