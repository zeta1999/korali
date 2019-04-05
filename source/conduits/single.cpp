#include "korali.h"

Korali::Conduit::Single::Single() : Korali::Conduit::Base::Base() {}

void Korali::Conduit::Single::initialize()
{
  // Allocating Global Pointer for Samples
 sampleArrayPointer  = (double*) calloc (_k->N*_k->S, sizeof(double));
 fitnessArrayPointer = (double*) calloc (_k->_referenceDataSize, sizeof(double));

 _k->Solver->run();
}

double* Korali::Conduit::Single::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::Single::evaluateSample(size_t sampleId)
{
 double fitness = _k->Problem->evaluateFitness(&sampleArrayPointer[_k->N*sampleId]);
 _k->Solver->processSample(sampleId, fitness);
}

double* Korali::Conduit::Single::getFitnessArrayPointer()
{
  return fitnessArrayPointer;
}

void Korali::Conduit::Single::checkProgress()
{

}
