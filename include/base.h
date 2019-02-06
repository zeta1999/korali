#ifndef _BASE_H_
#define _BASE_H_

#include "problem.h"
#include "mpi.h"
#include <upcxx/upcxx.hpp>
#include <queue>

namespace Korali
{

class KoraliBase
{
  public:

  // Public Methods
  KoraliBase(Problem* problem, MPI_Comm comm);

	void setLambda(size_t lambda) { _lambda = lambda; }
  void setMaxFitnessEvaluations(size_t maxFitnessEvaluations) { _maxFitnessEvaluations = maxFitnessEvaluations; }
  void setMaxGenerations(size_t maxGenerations) { _maxGenerations = maxGenerations; }

  void run();

	// These values should be protected, but they need to be public for RPCs to work
  MPI_Comm _comm;
  int _rankId;
  int _rankCount;

  Problem* _problem;
  double* _fitnessVector;
  bool _continueEvaluations;
  double* _samplePopulation;
  size_t _lambda; // Number of offspring per sample cycle

  std::queue<int> _workers;
  upcxx::future<> _bcastFuture;

  protected:

  size_t _maxFitnessEvaluations;   // Defines maximum number of fitness evaluations
  size_t _maxGenerations; // Defines maximum number of generations

  virtual void Korali_InitializeInternalVariables() = 0;
  virtual void Korali_PrintResults() = 0;
  virtual void Korali_UpdateDistribution(const double *fitnessVector) = 0;
  virtual void Korali_GetSamplePopulation() = 0;
  virtual bool Korali_CheckTermination() = 0;

  private:

  // Tasking Layer Methods
  void supervisorThread();
  void workerThread();
};

void workerEvaluateFitnessFunction(size_t position);
void workerComeback(int worker, size_t position, double fitness);
void broadcastSamples();
void finalizeEvaluation();

} // namespace Korali

extern Korali::KoraliBase* _kb;

#endif // _BASE_H_
