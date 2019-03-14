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

  _sampleSize = 1000;
	_maxGens = 20;

	N = _problem->_parameterCount;

  // Initialize Parameter Priors
  for (int i = 0; i < N; i++)
  	_problem->_parameters[i].initializePriorDistribution(_problem->_seed+i+1);
}

void Korali::Solver::run()
{
	_k = this;
	upcxx::init();
	_rankId = upcxx::rank_me();
	_rankCount = upcxx::rank_n();

  // Allocating Global Pointer for Samples
	if (_rankId == 0) sampleGlobalPtr  = upcxx::new_array<double>(N*_sampleSize);
	upcxx::broadcast(&sampleGlobalPtr,  1, 0).wait();

  if (_rankId == 0) supervisorThread(); else workerThread();

  upcxx::barrier();
  upcxx::finalize();
}

void Korali::Solver::supervisorThread()
{
  // Creating Worker Queue
  for (int i = 1; i < _rankCount; i++) _workers.push(i);

	supervisorThread2();

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
			 //printf("Worker %d: Evaluated [%f, %f] - Fitness: %f\n", _rankId, candidatePoint[0], candidatePoint[1], candidateFitness);
			upcxx::rpc_ff(0, [](size_t c, double fitness, int workerId){_k->processSample(c, fitness); _k->_workers.push(workerId); }, _sampleId, candidateFitness, _rankId);
		}
		 upcxx::progress();
	}
}
