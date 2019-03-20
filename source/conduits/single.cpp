#include "conduits/single.h"
#include "solvers/base.h"

Korali::Conduit::Single::Single(Korali::Solver::Base* solver) : Korali::Conduit::Base::Base(solver) {}

void Korali::Conduit::Single::initialize()
{
  // Allocating Global Pointer for Samples
	sampleArrayPointer  = (double*) calloc (_solver->N*_solver->_sampleCount, sizeof(double));
	fitnessArrayPointer = (double*) calloc (_solver->_problem->_referenceDataSize, sizeof(double));

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

double* Korali::Conduit::Single::getFitnessArrayPointer()
{
  return fitnessArrayPointer;
}

void Korali::Conduit::Single::checkProgress()
{

}
