#ifdef _KORALI_USE_OPENMP

#include "conduits/openmp.h"
#include "solvers/base.h"
#include "omp.h"

Korali::Conduit::OpenMP::OpenMP() : Korali::Conduit::Base::Base()
{
 _threadCount = 1;
  _continueEvaluations = true;
}

void Korali::Conduit::OpenMP::initialize()
{
 #pragma omp parallel
 _threadCount = omp_get_num_threads();

 if (_threadCount < 2) { fprintf(stderr, "[Korali] Error: Running Korali's OpenMP Conduit with less than 2 threads is not allowed.\n"); exit(-1); }

 // Allocating Global Pointer for Samples
 sampleArrayPointer  = (double*) calloc (Solver->N*Solver->_sampleCount, sizeof(double));
 fitnessArrayPointer = (double*) calloc (Solver->Problem->_referenceDataSize*_threadCount, sizeof(double));

 #pragma omp parallel
 {
  #pragma omp master
    supervisorThread();
  workerThread();
 }
}

void Korali::Conduit::OpenMP::supervisorThread()
{
 Solver->run();

 _continueEvaluations = false;
}

double* Korali::Conduit::OpenMP::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

double* Korali::Conduit::OpenMP::getFitnessArrayPointer()
{
 int threadId = omp_get_thread_num();
  return &fitnessArrayPointer[Solver->Problem->_referenceDataSize*threadId];
}

void Korali::Conduit::OpenMP::workerThread()
{
 while(_continueEvaluations)
 {
  bool foundSample = false;
   size_t sampleId = 0;

   if (_queueLock.trylock())
   {
    if (!_sampleQueue.empty())
    {
     sampleId = _sampleQueue.front();
     _sampleQueue.pop();
     foundSample = true;
    }
    _queueLock.unlock();
   }

  if (foundSample)
  {
   double candidateFitness = Solver->Problem->evaluateFitness(&sampleArrayPointer[Solver->N*sampleId]);
   //printf("Thread %d/%d: Sample: %ld, Fitness: %f\n", threadId, _threadCount, sampleId, candidateFitness);

   _queueLock.lock();
    Solver->processSample(sampleId, candidateFitness);
   _queueLock.unlock();

   //printf("Thread %d/%d: Updated\n", threadId, _threadCount);
  }
 }
}

void Korali::Conduit::OpenMP::evaluateSample(size_t sampleId)
{
 //printf("SampleId: %ld\n", sampleId);

 _queueLock.lock();
  _sampleQueue.push(sampleId);
 _queueLock.unlock();
}

void Korali::Conduit::OpenMP::checkProgress()
{
}

#endif // _KORALI_USE_OPENMP
