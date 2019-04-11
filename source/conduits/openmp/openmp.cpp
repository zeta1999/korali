#ifdef _KORALI_USE_OPENMP

#include "korali.h"

/************************************************************************/
/*                  Constructor / Destructor Methods                    */
/************************************************************************/

Korali::Conduit::OpenMP::OpenMP(nlohmann::json& js) : Korali::Conduit::Base::Base(js)
{
 setConfiguration(js);

#pragma omp parallel
 _threadCount = omp_get_num_threads();

 if (_threadCount < 2) { fprintf(stderr, "[Korali] Error: Running Korali's OpenMP Conduit with less than 2 threads is not allowed.\n"); exit(-1); }

 // Allocating Global Pointer for Samples
 sampleArrayPointer  = (double*) calloc (_solver->N*_solver->_sampleCount, sizeof(double));
 fitnessArrayPointer = (double*) calloc (_solver->_problem->_referenceDataSize*_threadCount, sizeof(double));

 #pragma omp parallel
 {
  #pragma omp master
    supervisorThread();
  workerThread();
 }
}

Korali::Conduit::OpenMP::~OpenMP()
{

}

/************************************************************************/
/*                    Configuration Methods                             */
/************************************************************************/

nlohmann::json Korali::Conduit::OpenMP::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "OpenMP"

 return js;
}

void Korali::Conduit::OpenMP::setConfiguration(nlohmann::json js)
{
 _threadCount = 1;
 _continueEvaluations = true;
}

/************************************************************************/
/*                    Functional Methods                                */
/************************************************************************/

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
   double candidateFitness = _solver->_problem->evaluateFitness(&sampleArrayPointer[_solver->N*sampleId]);
   //printf("Thread %d/%d: Sample: %ld, Fitness: %f\n", threadId, _threadCount, sampleId, candidateFitness);

   _queueLock.lock();
    _solver->processSample(sampleId, candidateFitness);
   _queueLock.unlock();

   //printf("Thread %d/%d: Updated\n", threadId, _threadCount);
  }
 }
}

void Korali::Conduit::OpenMP::evaluateSample(double* sampleArray, size_t sampleId)
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
