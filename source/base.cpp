#include "base.h"
#include <chrono>

Korali::KoraliBase* _kb;

Korali::KoraliBase::KoraliBase(ProblemBase* problem, MPI_Comm comm)
{
  _problem = problem;
	_comm = comm;

	_lambda = -1;
	_rankId = -1;
	_rankCount = -1;

	_maxFitnessEvaluations = 900*(_problem->_dimCount+3)*(_problem->_dimCount+3);
	_maxGenerations = std::numeric_limits<size_t>::max();

  _bcastFuture = upcxx::make_future();
  _continueEvaluations = true;
}

void Korali::KoraliBase::run()
{
	_kb = this;
	upcxx::init();
	_rankId = upcxx::rank_me();
	_rankCount = upcxx::rank_n();

  // Checking Lambda's value
  if(_lambda < 1 )  { fprintf( stderr, "[Korali] Error: Lambda (%lu) should be higher than one.\n", _lambda); exit(-1); }

  // Allocating sample matrix
  _samplePopulation = (double *) calloc (_kb->_problem->_dimCount*_kb->_lambda, sizeof(double));

  if (_rankId == 0) supervisorThread(); else workerThread();

	upcxx::barrier();
  upcxx::finalize();
}

void Korali::KoraliBase::workerThread()
{
	while(_continueEvaluations)
	{
		 upcxx::progress();
		 _bcastFuture.wait();
	}
}

void Korali::KoraliBase::supervisorThread()
{
	auto startTime = std::chrono::system_clock::now();
	for (int i = 0; i < _rankCount; i++) _workers.push(i);
	Korali_InitializeInternalVariables();
	_fitnessVector = (double*) calloc (sizeof(double), _lambda);

	while( !Korali_CheckTermination() )
	{
		upcxx::future<> futures = upcxx::make_future();

		Korali_GetSamplePopulation();
		for (int i = 1; i < _rankCount; i++) upcxx::rpc_ff(i, broadcastSamples);
		upcxx::broadcast(_samplePopulation, _problem->_dimCount*_lambda, 0).wait();

		for(int i = 0; i < _lambda; i++)
		{
			while(_workers.empty()) upcxx::progress();
			futures = upcxx::when_all(futures, upcxx::rpc(_workers.front(), workerEvaluateFitnessFunction, i));
			_workers.pop();
		}

		futures.wait();

		Korali_UpdateDistribution(_fitnessVector);
	}

	for (int i = 1; i < _rankCount; i++) upcxx::rpc_ff(i, finalizeEvaluation);

	auto endTime = std::chrono::system_clock::now();
	Korali_PrintResults(); // Printing Solver results
	printf("Total elapsed time = %.3lf  seconds\n", std::chrono::duration<double>(endTime-startTime).count());

}

void Korali::workerComeback(int worker, size_t position, double fitness)
{
	_kb->_fitnessVector[position] = fitness;
	_kb->_workers.push(worker);
}

void Korali::workerEvaluateFitnessFunction(size_t position)
{
	double fitness = _kb->_problem->evaluateFitness(&_kb->_samplePopulation[position*_kb->_problem->_dimCount]);
	upcxx::rpc_ff(0, workerComeback, _kb->_rankId, position, fitness);
}

void Korali::broadcastSamples() { _kb->_bcastFuture = upcxx::broadcast(_kb->_samplePopulation, _kb->_problem->_dimCount*_kb->_lambda, 0); }
void Korali::finalizeEvaluation() { _kb->_continueEvaluations = false; }
