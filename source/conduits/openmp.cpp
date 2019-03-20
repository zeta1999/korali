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
	#pragma omp parallel
	 _threadCount = omp_get_num_threads();

	printf("_threadCount: %d\n", _threadCount);
	if (_threadCount < 2) { fprintf(stderr, "[Korali] Error: Running Korali's OpenMP Conduit with less than 2 threads is not allowed.\n"); exit(-1); }

	// Allocating Global Pointer for Samples
	sampleArrayPointer  = (double*) calloc (_solver->N*_solver->_sampleCount, sizeof(double));

  #pragma omp parallel
	{
    #pragma omp master
      supervisorThread();
    workerThread();
	}

}

void Korali::Conduit::OpenMP::supervisorThread()
{
	printf("Only me: thread %d\n", omp_get_thread_num());
	_solver->runSolver();

	_continueEvaluations = false;
}

double* Korali::Conduit::OpenMP::getSampleArrayPointer()
{
  return sampleArrayPointer;
}

void Korali::Conduit::OpenMP::workerThread()
{
	while(_continueEvaluations)
	{
		bool _evaluateSample = false;
		size_t sampleId;

		#pragma omp critical (QueueUpdate)
		{
			if (!_pendingSamples.empty())
			{
				_evaluateSample = true;
				sampleId = _pendingSamples.front();
				_pendingSamples.pop();
			}
		}

		if (_evaluateSample)
		{
			double candidateFitness = _solver->_problem->evaluateFitness(&sampleArrayPointer[_solver->N*sampleId]);
			//printf("Thread %d/%d: Fitness: %f\n", omp_get_thread_num(), _threadCount, candidateFitness);
			#pragma omp critical (ProcessSample)
			 _solver->processSample(sampleId, candidateFitness);
		}
	}
}

void Korali::Conduit::OpenMP::evaluateSample(size_t sampleId)
{
	#pragma omp critical (QueueUpdate)
		_pendingSamples.push(sampleId);
}

void Korali::Conduit::OpenMP::checkProgress()
{
}
