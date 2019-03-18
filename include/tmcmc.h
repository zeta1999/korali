#ifndef _TMCMC_H_
#define _TMCMC_H_

#include "solver.h"

namespace Korali
{

typedef struct fparam_s {
    const double *fj;
    int           fn;
    double        pj;
    double        tol;
} fparam_t;

class TMCMC : public Solver
{
  public:

  // TMCMC Configuration
  int nChains;

  double TolCOV;              /* Target coefficient of variation of weights */
  double MinStep;             /* Min update of rho */
  double bbeta;               /* Covariance scaling parameter */

  double **local_cov;     /* [DATANUM][PROBDIM*PROBDIM] */
  bool _useLocalCov;

  // TMCMC Runtime Variables

  int     _currentGeneration;
  double  _varianceCoefficient;
  double  _annealingRatio;
  size_t  _uniqueSelections;
  size_t  _uniqueEntries;
  double  _logSelections;
  double  _acceptanceRate;
  double* _covarianceMatrix;            /*[PROBDIM][PROBDIM];*/
  double* _meanTheta;     /*[PROBDIM]*/
  bool    _verbose;

	// TMCMC Fields
	gsl_rng  *range;

	double* clPoints;   // Chain Leader Parameter Values
	double* clFitness;  // Chain Leader Fitness
	double* clLogPrior; // Chain Leader Log Prior

	double* ccPoints;   // Chain Candidate Parameter Values
	double* ccFitness;  // Chain Candidate Fitness
	double* ccLogPrior; // Chain Candidate Log Prior
	gsl_rng** chainGSLRange;

	bool*   chainPendingFitness; // Indicates that the fitness result for the chain is pending
	size_t  finishedChains;
	size_t* chainCurrentStep;
	size_t* chainLength;

	size_t  databaseEntries;
	double* databasePoints;
	double* databaseFitness;

  // Korali Methods

	TMCMC(Problem* problem, MPI_Comm comm = MPI_COMM_WORLD);

	void initializeEngine();
  void runEngine();

	// TMCMC Configuration Methods
	void setToleranceCOV(double TolCOV) { TolCOV = TolCOV; }
	void setUseLocalCOV(bool useLocalCov) { _useLocalCov = useLocalCov; }
	void setCovarianceScaling(double bbeta) { bbeta = bbeta; }

  // Internal TMCMC Methods
	void saveResults();
  void resampleGeneration();
  void updateDatabase(double* point, double fitness);
  void processSample(size_t c, double fitness);
  bool generateCandidate(int c);
  void computeChainCovariances(double** chain_cov, int newchains);
  void minSearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin);
  static double tmcmc_objlogp(double x, const double *fj, int fn, double pj, double zero);
  static double objLog(const gsl_vector *v, void *param);
};

} // namespace Korali

#endif // _TMCMC_H_
