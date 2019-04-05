#include "korali.h"

Korali::Conduit::Single::Single() : Korali::Conduit::Base::Base() {}

void Korali::Conduit::Single::initialize()
{
  // Allocating Global Pointer for Samples
 sampleArrayPointer  = (double*) calloc (_k->_parameterCount*_k->_sampleCount, sizeof(double));
 fitnessArrayPointer = (double*) calloc (_k->_referenceDataSize, sizeof(double));

 _k->_solver->run();
}

double* Korali::Conduit::Single::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::Single::evaluateSample(size_t sampleId)
{
 double fitness = _k->_problem->evaluateFitness(&sampleArrayPointer[_k->_solver->N*sampleId]);
 _k->_solver->processSample(sampleId, fitness);
}

double* Korali::Conduit::Single::getFitnessArrayPointer()
{
  return fitnessArrayPointer;
}

void Korali::Conduit::Single::checkProgress()
{

}
