#include "conduits/openmp.h"
#include "solvers/base.h"
#include "omp.h"

Korali::Conduit::OpenMP::OpenMP(Korali::Solver::Base* solver) : Korali::Conduit::Base::Base(solver)
{
	_threadCount = 1;
  _continueEvaluations = true;
}

void Korali::Conduit::OpenMP::initialize()
{
	int threadId;

	#pragma omp parallel
	 _threadCount = omp_get_num_threads();

	if (_threadCount < 2) { fprintf(stderr, "[Korali] Error: Running Korali's OpenMP Conduit with less than 2 threads is not allowed.\n"); exit(-1); }

	// Allocating Global Pointer for Samples
	sampleArrayPointer  = (double*) calloc (_solver->N*_solver->_sampleCount, sizeof(double));

	// Allocating sampleId array for workers
	_evaluateSampleId = (size_t*) calloc (_threadCount, sizeof(size_t));

	// Allocating evaluation flag array for workers
	_evaluateSampleFlag = (bool*) calloc (_threadCount, sizeof(bool));
	for (int i = 1; i < _threadCount; i++) _evaluateSampleFlag[i] = false;

  #pragma omp parallel
	{
    #pragma omp master
      supervisorThread();
    workerThread();
	}
}

void Korali::Conduit::OpenMP::supervisorThread()
{
  // Creating Worker Queue
  for (int i = 1; i < _threadCount; i++) _workers.push(i);

	_solver->runSolver();

	_continueEvaluations = false;
}

double* Korali::Conduit::OpenMP::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::OpenMP::workerThread()
{
	int threadId = omp_get_thread_num();

	while(_continueEvaluations)
	{
		if (_evaluateSampleFlag[threadId])
		{
			_evaluateSampleFlag[threadId] = false;
			double candidateFitness = _solver->_problem->evaluateFitness(&sampleArrayPointer[_solver->N*_evaluateSampleId[threadId]]);
			//printf("Thread %d/%d: Sample: %ld, Fitness: %f\n", threadId, _threadCount, _evaluateSampleId, candidateFitness);

			#pragma omp critical (DatabaseUpdate)
			 _solver->processSample(_evaluateSampleId[threadId], candidateFitness);

			 #pragma omp critical (QueueUpdate)
			 _workers.push(threadId);
		}
	}
}

void Korali::Conduit::OpenMP::evaluateSample(size_t sampleId)
{
	while(_workers.empty());

  #pragma omp critical (QueueUpdate)
	{
	 int workerId = _workers.front();
	 _workers.pop();
	 _evaluateSampleId[workerId] = sampleId; _evaluateSampleFlag[workerId] = true;
	}
}

void Korali::Conduit::OpenMP::checkProgress()
{
}
