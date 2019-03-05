#ifndef _CMAES_H_
#define _CMAES_H_

#include "mpi.h"
#include <upcxx/upcxx.hpp>
#include "problem.h"
#include <queue>
#include <chrono>

namespace Korali
{

class KoraliCMAES //: public KoraliBase
{
  public:

  // Public Methods
	KoraliCMAES(Problem* problem, MPI_Comm comm);

  // Runtime Methods (to be inherited from base class in the future)
  void run();
  void Korali_InitializeInternalVariables();
  void Korali_GetSamplePopulation();
  bool Korali_CheckTermination();
  void Korali_PrintResults();
  void Korali_UpdateDistribution(const double *fitnessVector);
  void Korali_SupervisorThread();
  void Korali_WorkerThread();

  static void workerEvaluateFitnessFunction(size_t position);
  static void workerComeback(int worker, size_t position, double fitness);
  static void broadcastSamples();
  static void finalizeEvaluation();

	// Configuration Methods
	void setLambda(size_t lambda) { _lambda = lambda; }
  void setMaxFitnessEvaluations(size_t maxFitnessEvaluations) { _maxFitnessEvaluations = maxFitnessEvaluations; }
  void setMaxGenerations(size_t maxGenerations) { _maxGenerations = maxGenerations; }
	void setDiagonalCovarianceMatrixEvalFrequency(size_t diagonalCovarianceMatrixEvalFrequency) { _diagonalCovarianceMatrixEvalFrequency = diagonalCovarianceMatrixEvalFrequency; }
	void setCovarianceEigensystemEvaluationFrequency(size_t covarianceEigensystemEvaluationFrequency) { _covarianceEigensystemEvaluationFrequency = covarianceEigensystemEvaluationFrequency; }
	void setStopFitnessEvalThreshold(double stopFitnessEvalThreshold) { _stopFitnessEvalThreshold = stopFitnessEvalThreshold; }
	void setStopFitnessDiffThreshold(double stopFitnessDiffThreshold) { _stopFitnessDiffThreshold = stopFitnessDiffThreshold; }
	void setStopFitnessDiffHistoryThreshold(double stopFitnessDiffHistoryThreshold) { _stopFitnessDiffHistoryThreshold = stopFitnessDiffHistoryThreshold; }
	void setStopMinDeltaX(double stopMinDeltaX) { _stopMinDeltaX = stopMinDeltaX; }
	void setStopMaxStdDevXFactor(double stopMaxStdDevXFactor) { _stopMaxStdDevXFactor = stopMaxStdDevXFactor; }
	void setStopMaxTimePerEigenDecomposition(double stopMaxTimePerEigendecomposition) { _stopMaxTimePerEigendecomposition = stopMaxTimePerEigendecomposition; }
	void setStopMinFitness(double stopMinFitness) { _stopMinFitness = stopMinFitness; }
	void setMu(size_t mu, std::string muType = "Logarithmic") { _mu = mu; _muType = muType; }
	void setMuCovariance(double muCovariance) { _muCovariance = muCovariance;}
	void setSigmaCumulationFactor(double sigmaCumulationFactor) { _sigmaCumulationFactor = sigmaCumulationFactor;}
	void setDampingFactor(double dampFactor) { _dampFactor = dampFactor; }
	void setCumulativeCovariance(double cumulativeCovariance) { _cumulativeCovariance = cumulativeCovariance; }
	void setCovarianceMatrixLearningRate(double covarianceMatrixLearningRate) { _covarianceMatrixLearningRate = covarianceMatrixLearningRate; }

  private:

	// Korali Runtime Variables
  MPI_Comm _comm;
  int _rankId;
  int _rankCount;

  Problem* _problem;
  double* _fitnessVector;
  bool _continueEvaluations;
  double* _samplePopulation;

  std::queue<int> _workers;
  upcxx::future<> _bcastFuture;

  // Configuration Variables
	size_t _lambda; // Number of offspring per sample cycle
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
  size_t _covarianceEigensystemEvaluationFrequency;

  // Stop conditions
  size_t _maxFitnessEvaluations;   // Defines maximum number of fitness evaluations
  size_t _maxGenerations; // Defines maximum number of generations
  double _stopFitnessEvalThreshold; // Defines minimum function value below which it stops
  double _stopFitnessDiffThreshold; // Defines minimum function value differences before stopping
  double _stopFitnessDiffHistoryThreshold; // Defines minimum function value differences among best values before stopping
  double _stopMinDeltaX; // Defines minimum delta of input parameters among generations before it stops.
  double _stopMaxStdDevXFactor; // Defines maximum standard deviation before it stops.
  double _stopMaxTimePerEigendecomposition; // Defines maximum time to be spent on eigensystem decomposition
  double _stopMinFitness; // Defines the minimum fitness allowed, otherwise it stops

  // Private CMAES-Specific Variables
  double sigma;  /* step size */
  Distribution* _gaussianGenerator;

  double *rgxmean;  /* mean x vector, "parent" */
  double *rgxbestever;
  double *curBest;
  int *index;       /* sorting index of sample pop. */
  double *arFuncValueHist;

  double chiN;
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

  double gen; /* Generation number */
  double countevals;
  double state; /* 1 == sampled, 2 == not in use anymore, 3 == updated */

  double maxdiagC; /* repeatedly used for output */
  double mindiagC;
  double maxEW;
  double minEW;

  short flgEigensysIsUptodate;
  double genOfEigensysUpdate;
  double dMaxSignifKond;

  // Private CMAES-Specific Methods
  void cmaes_reSampleSingle(int iindex);
  void cmaes_adaptC2(int hsig);
  void cmaes_testMinStdDevs();
  double cmaes_function_value_difference();
  void cmaes_updateEigensystem(int flgforce);
  void cmaes_eigen( int N,  double **C, double *diag, double **Q);
  int cmaes_maxIdx(const double *rgd, int len);
  int cmaes_minIdx(const double *rgd, int len);
  void cmaes_sorted_index(const double *rgFunVal, int *iindex, int n);
  bool cmaes_isFeasible(double *pop);
  double cmaes_doubleRangeMax(const double *rgd, int len);
  double cmaes_doubleRangeMin(const double *rgd, int len );
};

} // namespace Korali

extern Korali::KoraliCMAES* _kc;

#endif // _CMAES_H_
