#ifndef _KORALI_CMAES_H_
#define _KORALI_CMAES_H_

#include "solvers/base/base.h"
#include "parameters/gaussian/gaussian.h"
#include <chrono>
#include <map>

namespace Korali::Solver
{

class CMAES : public Korali::Solver::Base
{
 public:

 // Constructor / Destructor
 CMAES(nlohmann::json& js);
 ~CMAES();

 // Runtime Methods (to be inherited from base class in the future)
 void prepareGeneration();
 bool checkTermination();
 void updateDistribution(const double *fitnessVector);
 void run();
 void processSample(size_t sampleId, double fitness);

 // Serialization Methods
 nlohmann::json getConfiguration();
 void setConfiguration(nlohmann::json& js);
 void setState(nlohmann::json& js);

 private:

 // Korali Runtime Variables
 double* _fitnessVector;
 double* _samplePopulation;
 size_t _currentGeneration;
 bool* _initializedSample;
 char _terminationReason[500];
 char filepath[500];

 size_t _finishedSamples;
 size_t _s;
 size_t _mu;
 std::string _muType;
 double* _muWeights;
 double _muEffective;
 double _muCovariance;

 double _sigmaCumulationFactor;
 double _dampFactor;
 double _cumulativeCovariance;
 double _covarianceMatrixLearningRate;

 size_t _diagonalCovarianceMatrixEvalFrequency;
 size_t _covarianceEigenEvalFreq;

 // Stop conditions
 size_t _maxFitnessEvaluations;   // Defines maximum number of fitness evaluations
 double _stopFitnessEvalThreshold; // Defines minimum function value below which it stops
 double _stopFitnessDiffThreshold; // Defines minimum function value differences before stopping
 double _stopMinDeltaX; // Defines minimum delta of input parameters among generations before it stops.
 double _stopMaxStdDevXFactor; // Defines maximum standard deviation before it stops.
 double _stopMinFitness; // Defines the minimum fitness allowed, otherwise it stops
 size_t _maxGenenerations;

 // Private CMAES-Specific Variables
 double sigma;  /* step size */
 Parameter::Gaussian* _gaussianGenerator;

 double currentBest;
 double *rgxmean;  /* mean x vector, "parent" */
 double *rgxbestever;
 double *curBest;
 int *index;       /* sorting index of sample pop. */
 double currentFunctionValue;
 double prevFunctionValue;

 double **C;  /* lower triangular matrix: i>=j for C[i][j] */
 double **B;  /* matrix with normalize eigenvectors in columns */
 double *rgD; /* axis lengths */

 double *rgpc;
 double *rgps;
 double *rgxold;
 double *rgout;
 double *rgBDz;   /* for B*D*z */
 double *rgdTmp;  /* temporary (random) vector used in different places */
 double *rgFuncValue;

 size_t countevals;
 double maxdiagC; /* repeatedly used for output */
 double mindiagC;
 double maxEW;
 double minEW;

 bool flgEigensysIsUptodate;

 // Private CMAES-Specific Methods
 void reSampleSingle(size_t idx);
 void adaptC2(int hsig);
 double function_value_difference();
 void updateEigensystem(int flgforce);
 void eigen(size_t N,  double **C, double *diag, double **Q);
 int maxIdx(const double *rgd, int len);
 int minIdx(const double *rgd, int len);
 void sorted_index(const double *rgFunVal, int *iindex, int n);
 bool isFeasible(double *pop);
 double doubleRangeMax(const double *rgd, int len);
 double doubleRangeMin(const double *rgd, int len );
};

} // namespace Korali

#endif // _KORALI_CMAES_H_
