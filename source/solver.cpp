#include "solver.h"

Korali::Solver* _k;

Korali::Solver::Solver(Problem* problem, MPI_Comm comm)
{
  _problem = problem;
	_comm = comm;

	_rankId = 0;
	_rankCount = 1;
  _continueEvaluations = true;
	_evaluateSample = false;
  _verbose = false;

  _sampleCount = 1000;
	_maxGens = 200;

	N = _problem->_parameterCount;

  // Initialize Parameter Priors
  for (int i = 0; i < N; i++)
  	_problem->_parameters[i].initialize(_problem->_seed+i+1);
}

void Korali::Solver::run()
{
	_k = this;
	upcxx::init();
	_rankId = upcxx::rank_me();
	_rankCount = upcxx::rank_n();

  // Allocating Global Pointer for Samples
	if (_rankId == 0) sampleGlobalPtr  = upcxx::new_array<double>(N*_sampleCount);
	upcxx::broadcast(&sampleGlobalPtr,  1, 0).wait();

	// Checking correct parameters for problem
	for (int i = 0; i < N; i++) _problem->_parameters[i].checkBounds();

  if (_rankId == 0) supervisorThread(); else workerThread();

  upcxx::barrier();
  upcxx::finalize();
}

void Korali::Solver::supervisorThread()
{
  // Creating Worker Queue
  for (int i = 1; i < _rankCount; i++) _workers.push(i);

	runEngine();

  for (int i = 1; i < _rankCount; i++) upcxx::rpc_ff(i, [](){_k->_continueEvaluations = false;});
}

void Korali::Solver::workerThread()
{
	while(_continueEvaluations)
	{
		if (_evaluateSample)
		{
			_evaluateSample = false;
			double candidatePoint[N];
			upcxx::rget(sampleGlobalPtr + _sampleId*N, candidatePoint, N).wait();
			double candidateFitness = _problem->evaluateFitness(candidatePoint);
//			 printf("Worker %d: Evaluated %ld:[%f, %f] - Fitness: %f\n", _rankId, _sampleId, candidatePoint[0], candidatePoint[1], candidateFitness);
			upcxx::rpc_ff(0, [](size_t c, double fitness, int workerId){_k->processSample(c, fitness); _k->_workers.push(workerId); }, _sampleId, candidateFitness, _rankId);
		}
		 upcxx::progress();
	}
}

void Korali::Solver::evaluateSample(size_t sampleId)
{
	if (_rankCount == 1) { _k->processSample(sampleId, _problem->evaluateFitness(sampleGlobalPtr.local() + sampleId*N)); return; }// if Single core run
	while(_workers.empty()) upcxx::progress();
	int workerId = _workers.front(); _workers.pop();
	upcxx::rpc_ff(workerId, [](size_t c){_k->_sampleId = c; _k->_evaluateSample = true;}, sampleId);
}

void Korali::Solver::checkProgress()
{
	upcxx::progress();
}
