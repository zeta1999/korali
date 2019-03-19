#ifdef KORALI_UPCXX_CONDUIT
#include "conduits/upcxx.h"

Korali::Conduit::Conduit(BaseProblem* problem) : Korali::BaseConduit::BaseConduit(problem)
{
	_rankId = 0;
	_rankCount = 1;
  _continueEvaluations = true;
	_evaluateSample = false;
}

void Korali::Conduit::run()
{
	_k = this;
	this->Korali::BaseConduit::run();

	upcxx::init();
	_rankId = upcxx::rank_me();
	_rankCount = upcxx::rank_n();

	if (_rankCount < 2) { fprintf(stderr, "[Korali] Error: Running Korali's UPCxx Conduit with less than 2 ranks is not allowed.\n"); exit(-1); }

  // Allocating Global Pointer for Samples
	if (_rankId == 0) sampleGlobalPtr  = upcxx::new_array<double>(N*_sampleCount);
	upcxx::broadcast(&sampleGlobalPtr,  1, 0).wait();

  if (_rankId == 0) supervisorThread(); else workerThread();

  upcxx::barrier();
  upcxx::finalize();
}

void Korali::Conduit::supervisorThread()
{
  // Creating Worker Queue
  for (int i = 1; i < _rankCount; i++) _workers.push(i);

	runSolver();

  for (int i = 1; i < _rankCount; i++) upcxx::rpc_ff(i, [](){_k->_continueEvaluations = false;});
}

double* Korali::Conduit::getSampleArrayPointer()
{
  return sampleGlobalPtr.local();
}

void Korali::Conduit::workerThread()
{
	while(_continueEvaluations)
	{
		if (_evaluateSample)
		{
			_evaluateSample = false;
			double candidatePoint[N];
			upcxx::rget(sampleGlobalPtr + _sampleId*N, candidatePoint, N).wait();
			double candidateFitness = _problem->evaluateFitness(candidatePoint);
			//printf("Worker %d: Evaluated %ld:[%f, %f] - Fitness: %f\n", _rankId, _sampleId, candidatePoint[0], candidatePoint[1], candidateFitness);
			upcxx::rpc_ff(0, [](size_t c, double fitness, int workerId){_k->processSample(c, fitness); _k->_workers.push(workerId); }, _sampleId, candidateFitness, _rankId);
		}
		 upcxx::progress();
	}
}

void Korali::Conduit::evaluateSample(size_t sampleId)
{
	if (_rankCount == 1) { _k->processSample(sampleId, _problem->evaluateFitness(sampleGlobalPtr.local() + sampleId*N)); return; }// if Single core run
	while(_workers.empty()) upcxx::progress();
	int workerId = _workers.front(); _workers.pop();
	upcxx::rpc_ff(workerId, [](size_t c){_k->_sampleId = c; _k->_evaluateSample = true;}, sampleId);
}

void Korali::Conduit::checkProgress()
{
	upcxx::progress();
}
#endif
