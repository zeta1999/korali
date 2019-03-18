#ifdef _KORALI_SINGLE_CONDUIT
#include "conduits/single.h"

Korali::Conduit::Conduit(Problem* problem) : Korali::BaseConduit::BaseConduit (problem) {}

void Korali::Conduit::run()
{
	_k = this;
	_k->Korali::BaseConduit::run();

  #ifndef _KORALI_SINGLE_CONDUIT
		fprintf(stderr, "[Korali] Warning: No Korali communication conduit was set.\n");
		fprintf(stderr, "[Korali] Use: $export KORALI_CONDUIT=conduit.\n");
		fprintf(stderr, "[Korali] Where conduit = {mpi, upcxx, single}, and recompile.\n");
		fprintf(stderr, "[Korali] Defaulting to: single\n");
  #endif

  // Allocating Global Pointer for Samples
	sampleArrayPointer  = (double*) calloc (N*_sampleCount, sizeof(double));

	runSolver();
}

double* Korali::Conduit::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::evaluateSample(size_t sampleId)
{
	double fitness = _problem->evaluateFitness(&sampleArrayPointer[N*sampleId]);
	processSample(sampleId, fitness);
}

void Korali::Conduit::checkProgress()
{

}
#endif
