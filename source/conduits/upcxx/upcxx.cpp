#ifdef _KORALI_USE_UPCXX

#include "korali.h"

using json = nlohmann::json;

Korali::Conduit::UPCXX* _ux;

Korali::Conduit::UPCXX::UPCXX() : Korali::Conduit::Base::Base()
{
 _rankId = 0;
 _rankCount = 1;
  _continueEvaluations = true;
 _evaluateSample = false;
}

void Korali::Conduit::UPCXX::initialize()
{
 _ux = this;
 _rankId = upcxx::rank_me();
 _rankCount = upcxx::rank_n();

 if (_rankCount < 2)
 {
  fprintf(stderr, "[Korali] Error: Running Korali's UPCxx Conduit with less than 2 ranks is not allowed.\n");
  exit(-1);
 }

 // Allocating Global Pointer for Samples
 // fitnessArrayPointer = (double*) calloc (_k->_referenceDataSize, sizeof(double));
 if (_rankId == 0) sampleGlobalPtr  = upcxx::new_array<double>(_k->N*_k->S);
 upcxx::broadcast(&sampleGlobalPtr,  1, 0).wait();
}

void Korali::Conduit::UPCXX::run()
{
  if (_rankId == 0) supervisorThread();
  else workerThread();

  upcxx::barrier();
}

void Korali::Conduit::UPCXX::supervisorThread()
{
  // Creating Worker Queue
  for (int i = 1; i < _rankCount; i++) _workers.push(i);

 _k->_solver->run();

  for (int i = 1; i < _rankCount; i++) upcxx::rpc_ff(i, [](){_ux->_continueEvaluations = false;});
}

double* Korali::Conduit::UPCXX::getSampleArrayPointer()
{
  return sampleGlobalPtr.local();
}

void Korali::Conduit::UPCXX::workerThread()
{
 while(_continueEvaluations)
 {
  if (_evaluateSample)
  {
   _evaluateSample = false;
   double candidatePoint[_k->N];
   upcxx::rget(sampleGlobalPtr + _sampleId*_k->N, candidatePoint, _k->N).wait();
   double candidateFitness = _k->_problem->evaluateFitness(candidatePoint);
   //printf("Worker %d: Evaluated %ld:[%f, %f] - Fitness: %f\n", _rankId, _sampleId, candidatePoint[0], candidatePoint[1], candidateFitness);
   upcxx::rpc_ff(0, [](size_t c, double fitness, int workerId){_k->_solver->processSample(c, fitness); _ux->_workers.push(workerId); }, _sampleId, candidateFitness, _rankId);
  }
  upcxx::progress();
 }
}

void Korali::Conduit::UPCXX::evaluateSample(size_t sampleId)
{
 while(_workers.empty()) upcxx::progress();
 int workerId = _workers.front(); _workers.pop();
 upcxx::rpc_ff(workerId, [](size_t c){_ux->_sampleId = c; _ux->_evaluateSample = true;}, sampleId);
}

void Korali::Conduit::UPCXX::checkProgress()
{
 upcxx::progress();
}

json Korali::Conduit::UPCXX::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();
 return js;
}

void Korali::Conduit::UPCXX::setConfiguration(json js)
{
	this->Korali::Conduit::Base::setConfiguration(js);
}


#endif // #ifdef _KORALI_USE_UPCXX
