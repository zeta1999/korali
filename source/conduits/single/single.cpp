#include "korali.h"

void Korali::Conduit::Single::initialize()
{
}

void Korali::Conduit::Single::run()
{
	 _k->_solver->run();
}

void Korali::Conduit::Single::evaluateSample(double* sampleArray, size_t sampleId)
{
 double fitness = _k->_problem->evaluateFitness(&sampleArray[_k->N*sampleId]);
 _k->_solver->processSample(sampleId, fitness);
}

void Korali::Conduit::Single::checkProgress()
{

}

