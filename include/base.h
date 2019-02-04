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
  Dimension* getDimension(int dim);
  Dimension* operator[] (int x);
  void setMaxFitnessEvaluations(size_t maxFitnessEvaluations);
  void setMaxGenerations(size_t maxGenerations);

  virtual void run() = 0;

  protected:

  // Dimesion, Fitness, and Distribution Variables
	size_t _seed;
	size_t _dimCount;
  Dimension* _dims;
  Distribution* _gaussianGenerator;

  double (*_fitnessFunction) (double*, int);
  double* _fitnessVector;
  double** _samplePopulation;

  size_t _maxFitnessEvaluations;   // Defines maximum number of fitness evaluations
  size_t _maxGenerations; // Defines maximum number of generations

  virtual void initializeInternalVariables() = 0;
  virtual int is_feasible(double *pop) = 0;
  virtual double evaluate_population() = 0;
};

} // namespace Korali

#endif // _BASE_H_
