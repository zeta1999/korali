#ifndef _CMAES_H_
#define _CMAES_H_

#include "base.h"

namespace Korali
{

class KoraliCMAES : public KoraliBase
{
  public:

  // Public Methods
	KoraliCMAES(Problem* problem, MPI_Comm comm);

	void setDiagonalCovarianceMatrixEvalFrequency(size_t diagonalCovarianceMatrixEvalFrequency) { _diagonalCovarianceMatrixEvalFrequency = diagonalCovarianceMatrixEvalFrequency; }
	void setCovarianceEigensystemEvaluationFrequency(size_t covarianceEigensystemEvaluationFrequency) { _covarianceEigensystemEvaluationFrequency = covarianceEigensystemEvaluationFrequency; }
	void setStopFitnessEvalThreshold(double stopFitnessEvalThreshold) { _stopFitnessEvalThreshold = stopFitnessEvalThreshold; }
	void setStopFitnessDiffThreshold(double stopFitnessDiffThreshold) { _stopFitnessDiffThreshold = stopFitnessDiffThreshold; }
	void setStopFitnessDiffHistoryThreshold(double stopFitnessDiffHistoryThreshold) { _stopFitnessDiffHistoryThreshold = stopFitnessDiffHistoryThreshold; }
	void setStopMinDeltaX(double stopMinDeltaX) { _stopMinDeltaX = stopMinDeltaX; }
	void setStopMaxStdDevXFactor(double stopMaxStdDevXFactor) { _stopMaxStdDevXFactor = stopMaxStdDevXFactor; }
	void setStopMaxTimePerEigenDecomposition(double stopMaxTimePerEigendecomposition) { _stopMaxTimePerEigendecomposition = stopMaxTimePerEigendecomposition; }
	void setStopMinFitness(double stopMinFitness) { _stopMinFitness = stopMinFitness; }
	void setMu(size_t mu, std::string muType) { _mu = mu; _muType = muType; }
	void setMuCovariance(double muCovariance) { _muCovariance = muCovariance;}
	void setSigmaCumulationFactor(double sigmaCumulationFactor) { _sigmaCumulationFactor = sigmaCumulationFactor;}
	void setDampingFactor(double dampFactor) { _dampFactor = dampFactor; }
	void setCumulativeCovariance(double cumulativeCovariance) { _cumulativeCovariance = cumulativeCovariance; }
	void setCovarianceMatrixLearningRate(double covarianceMatrixLearningRate) { _covarianceMatrixLearningRate = covarianceMatrixLearningRate; }

  private:

  // Configuration Variables

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

  // Overriding Base Korali Class virtual methods
  void Korali_InitializeInternalVariables();
  void Korali_GetSamplePopulation();
  bool Korali_CheckTermination();
  void Korali_PrintResults();
  void Korali_UpdateDistribution(const double *fitnessVector);

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

#endif // _CMAES_H_
