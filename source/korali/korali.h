#ifndef _KORALI_H_
#define _KORALI_H_

#include "dimension.h"

namespace Korali
{

class KoraliBase
{
  public:
  Dimension* _dims;
  double (*_fitfunction) (double*, int);

  size_t _maxFitnessEvaluations;   // Defines maximum number of fitness evaluations
  size_t _maxGenerations; // Defines maximum number of generations
  size_t _lambda; // Number of offspring per sample cycle

  // Defining stop conditions
  double _stopFitnessEvalThreshold; // Defines minimum function value below which it stops
  double _stopFitnessDiffThreshold; // Defines minimum function value differences before stopping
  double _stopFitnessDiffHistoryThreshold; // Defines minimum function value differences among best values before stopping
  double _stopMinDeltaX; // Defines minimum delta of input parameters among generations before it stops.
  double _stopMaxStdDevXFactor; // Defines maximum standard deviation before it stops.
  double _stopMaxTimePerEigendecomposition; // Defines maximum time to be spent on eigensystem decomposition


  KoraliBase(int dim, double (*fun) (double*, int), int seed);

  Dimension* getDimension(int dim);
  Dimension* operator[] (int x);
  void setMaxFitnessEvaluations(size_t maxFitnessEvaluations);
  void setMaxGenerations(size_t maxGenerations);

  void setLambda(size_t lambda);
  void setStopFitnessEvalThreshold(double stopFitnessEvalThreshold);
  void setStopFitnessDiffThreshold(double stopFitnessDiffThreshold);
  void setStopFitnessDiffHistoryThreshold(double stopFitnessDiffHistoryThreshold);
  void setStopMinDeltaX(double stopMinDeltaX);
  void setStopMaxStdDevXFactor(double stopMaxStdDevXFactor);
  void setStopMaxTimePerEigenDecomposition(double stopMaxTimePerEigendecomposition);

};


} // namespace Korali

extern Korali::KoraliBase* k;

#endif // _KORALI_H_
