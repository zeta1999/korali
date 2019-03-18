#include "engines/single.h"

Korali::Engine::Engine(Problem* problem) : Korali::BaseEngine::BaseEngine (problem) {}

void Korali::Engine::run()
{
	_k = this;
	_k->Korali::BaseEngine::run();

  #ifndef _KORALI_SINGLE_ENGINE
		fprintf(stderr, "[Korali] Warning: No Korali communication engine was set.\n");
		fprintf(stderr, "[Korali] Use: $export KORALI_ENGINE=engine.\n");
		fprintf(stderr, "[Korali] Where engine = {mpi, upcxx, single}, and recompile.\n");
		fprintf(stderr, "[Korali] Defaulting to: single\n");
  #endif

  // Allocating Global Pointer for Samples
	sampleArrayPointer  = (double*) calloc (N*_sampleCount, sizeof(double));

	runEngine();
}

double* Korali::Engine::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Engine::evaluateSample(size_t sampleId)
{
	double fitness = _problem->evaluateFitness(&sampleArrayPointer[N*sampleId]);
	processSample(sampleId, fitness);
}

void Korali::Engine::checkProgress()
{

}
