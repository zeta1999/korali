#ifndef _BASE_H_
#define _BASE_H_

#include "dimension.h"
#include <upcxx/upcxx.hpp>
#include <queue>

namespace Korali
{

class KoraliBase
{
  public:

  // Public Methods
  KoraliBase(size_t dim, double (*fun) (double*, int), size_t seed);
  double getTotalDensityLog(double* x);
  double getTotalDensity(double* x);

  void setMaxFitnessEvaluations(size_t maxFitnessEvaluations) { _maxFitnessEvaluations = maxFitnessEvaluations; }
  void setMaxGenerations(size_t maxGenerations) { _maxGenerations = maxGenerations; }
  Dimension* getDimension(int dim) { return &_dims[dim]; }
  Dimension* operator[](int dim) { return getDimension(dim); }
	void setLambda(size_t lambda) { _lambda = lambda; }

	// These values should be protected, but they need to be public for RPCs to work
  double* _fitnessVector;
	std::queue<int> _workers;
	size_t _dimCount;
  double (*_fitnessFunction) (double*, int);
  size_t _rankId;
  bool _continueEvaluations;

  void run();

  protected:

  // Dimesion, Fitness, and Distribution Variables
	size_t _seed;
  size_t _lambda; // Number of offspring per sample cycle
  size_t _generation;
  Dimension* _dims;
  Distribution* _gaussianGenerator;
  double** _samplePopulation;
  size_t _maxFitnessEvaluations;   // Defines maximum number of fitness evaluations
  size_t _maxGenerations; // Defines maximum number of generations

  virtual void Korali_InitializeInternalVariables() = 0;
  virtual void Korali_PrintResults() = 0;
  virtual double* Korali_UpdateDistribution(const double *fitnessVector) = 0;
  virtual double** Korali_GetSamplePopulation() = 0;
  virtual bool Korali_CheckTermination() = 0;

  private:

  // Tasking Layer Methods
  void supervisorThread();
  void workerThread();
};

void workerEvaluateFitnessFunction(size_t position, double d0, double d1, double d2, double d3);
void workerComeback(size_t worker, size_t position, double fitness);
void finalizeEvaluation();

} // namespace Korali

extern Korali::KoraliBase* __kbRuntime;

#endif // _BASE_H_
