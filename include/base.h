#ifndef _BASE_H_
#define _BASE_H_

#include "dimension.h"
#include "mpi.h"
#include <upcxx/upcxx.hpp>
#include <queue>

namespace Korali
{

class KoraliBase
{
  public:

  // Public Methods
  KoraliBase(size_t dim, double (*fun) (double*, int), size_t seed, MPI_Comm comm);
  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);

  void setMaxFitnessEvaluations(size_t maxFitnessEvaluations) { _maxFitnessEvaluations = maxFitnessEvaluations; }
  void setMaxGenerations(size_t maxGenerations) { _maxGenerations = maxGenerations; }
  Dimension* getDimension(int dim) { return &_dims[dim]; }
  Dimension* operator[](int dim) { return getDimension(dim); }
	void setLambda(size_t lambda) { _lambda = lambda; }

  void run();

	// These values should be protected, but they need to be public for RPCs to work
  MPI_Comm _comm;
  int _rankId;
  int _rankCount;
  double* _fitnessVector;
  bool _continueEvaluations;
  double* _samplePopulation;
  double (*_fitnessFunction) (double*, int);
  std::queue<int> _workers;
  upcxx::future<> _bcastFuture;

  // Dimesion, Fitness, and Distribution Variables
	size_t _seed;
	size_t _dimCount;
  size_t _lambda; // Number of offspring per sample cycle

  protected:

  Dimension* _dims;
  Distribution* _gaussianGenerator;

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
