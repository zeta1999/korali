#ifndef _BASE_H_
#define _BASE_H_

#include "dimension.h"

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

  void run();

  protected:

  // Dimesion, Fitness, and Distribution Variables
	size_t _seed;
	size_t _dimCount;
  size_t _lambda; // Number of offspring per sample cycle
  Dimension* _dims;
  Distribution* _gaussianGenerator;

  double (*_fitnessFunction) (double*, int);
  double* _fitnessVector;
  double** _samplePopulation;

  size_t _maxFitnessEvaluations;   // Defines maximum number of fitness evaluations
  size_t _maxGenerations; // Defines maximum number of generations

  virtual void Korali_InitializeInternalVariables() = 0;
  virtual void Korali_PrintResults() = 0;
  virtual double* Korali_UpdateDistribution(const double *fitnessVector) = 0;
  virtual double** Korali_GetSamplePopulation() = 0;
  virtual bool Korali_CheckTermination() = 0;
};

} // namespace Korali

#endif // _BASE_H_
