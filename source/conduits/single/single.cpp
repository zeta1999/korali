#include "korali.h"

using json = nlohmann::json;

Korali::Conduit::Single::Single() : Korali::Conduit::Base::Base() {}

void Korali::Conduit::Single::initialize()
{
  // Allocating Global Pointer for Samples
 sampleArrayPointer  = (double*) calloc (_k->N*_k->S, sizeof(double));
// fitnessArrayPointer = (double*) calloc (_k->_referenceDataSize, sizeof(double));
}

void Korali::Conduit::Single::run()
{
	 _k->_solver->run();
}

double* Korali::Conduit::Single::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::Single::evaluateSample(size_t sampleId)
{
 double fitness = _k->_problem->evaluateFitness(&sampleArrayPointer[_k->N*sampleId]);
 _k->_solver->processSample(sampleId, fitness);
}

void Korali::Conduit::Single::checkProgress()
{

}

json Korali::Conduit::Single::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();
 return js;
}

void Korali::Conduit::Single::setConfiguration(json js)
{
	this->Korali::Conduit::Base::setConfiguration(js);
}
