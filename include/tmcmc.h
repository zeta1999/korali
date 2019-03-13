#ifndef _TMCMC_H_
#define _TMCMC_H_

#include <stdlib.h>
#include "mpi.h"
#include <upcxx/upcxx.hpp>
#include "problem.h"
#include <queue>

namespace Korali
{

struct optim_options {
    size_t MaxIter;             /* Max number of search iterations */
    double Tol;                 /* Tolerance for root finding */
    bool    Display;             /* Print output */
    double Step;                /* Search stepsize */
    double LowerBound;          /* Lower bound for root finding (fmincon & fzerosearch)*/
    double UpperBound;          /* Upper bound for root finding (fmincon & fzerosearch)*/
};

typedef struct runinfo_t {
    int    Gen;
    double CoefVar;
    double p;
    int    currentuniques;
    double logselections;
    double acceptance;
    double *SS;            /*[PROBDIM][PROBDIM];*/
    double *meantheta;     /*[PROBDIM]*/
} runinfo_t;


typedef struct fparam_s {
    const double *fj;
    int           fn;
    double        pj;
    double        tol;
} fparam_t;


typedef struct sort_s {
    int idx;
    int nsteps;
    double F;
} sort_t;


class KoraliTMCMC // : public KoraliBase
{
  public:

	// Korali Runtime Variables
  MPI_Comm _comm;
  int _rankId;
  int _rankCount;

  Problem* _problem;
  bool _continueEvaluations;
  bool _isInitialization;

  upcxx::future<> _bcastFuture;

  // TMCMC Configuration

  size_t _popSize; // Number of offspring per sample cycle

  int MaxStages;                  /* Max number of tmcmc generations */
  int nChains;

  int MinChainLength;         /* MinChainLength > 0: setting MinChainLength */
  int MaxChainLength;         /* MaxChainLength > 0: splitting long chains */

  double TolCOV;              /* Target coefficient of variation of weights */
  double MinStep;             /* Min update of rho */
  double bbeta;               /* Covariance scaling parameter */

  optim_options options;      /* Optimization options (see above) */

  double **local_cov;     /* [DATANUM][PROBDIM*PROBDIM] */
  bool use_local_cov;

  runinfo_t runinfo;

	// TMCMC Fields
	gsl_rng  *range;


	double* chainPoints;
	double* clPoints;   // Chain Leader Parameter Values
	double* clFitness;  // Chain Leader Fitness
	double* clLogPrior; // Chain Leader Log Prior

	upcxx::global_ptr<double> ccPointsGlobalPtr; // Global Pointer for Chain Candidate Parameters
	double* ccPoints;   // Chain Candidate Parameter Values
	double* ccFitness;  // Chain Candidate Fitness
	double* ccLogPrior; // Chain Candidate Log Prior
	bool*   ccSuitable;   // Indicates whether the candidate is suitable.
	gsl_rng** chainGSLRange;

	bool*   chainPendingFitness; // Indicates that the fitness result for the chain is pending
	size_t  finishedChains;
	size_t* chainCurrentStep;
	size_t* chainLength;
	size_t  N; // Parameter Count

	size_t  databaseEntries;
	double* databasePoints;
	double* databaseFitness;

	// Worker Parameters
	std::queue<int> _workers;
	size_t _nextChainEval;
	bool   _evaluateChain;

  // Korali Engine Methods
	KoraliTMCMC(Problem* problem, MPI_Comm comm = MPI_COMM_WORLD);
	void run();

	bool Korali_VerifyParameters(char* errorString);
	void Korali_InitializeInternalVariables();
  void Korali_GetSamplePopulation();
  bool Korali_CheckTermination(){if (runinfo.Gen++ == 0) return false; return true;};
  void Korali_PrintResults(){};
  void Korali_UpdateDistribution(const double *fitnessVector){};
  void Korali_SupervisorThread();
  void Korali_WorkerThread();

	// TMCMC Configuration Methods
	void setPopulationSize(int popSize) { _popSize = popSize; }
	void setMaxStages(int MaxStages) { MaxStages = MaxStages; }
	void setToleranceCOV(double TolCOV) { TolCOV = TolCOV; }
	void setUseLocalCOV(double use_local_cov) { use_local_cov = use_local_cov; }
	void setCovarianceScaling(double bbeta) { bbeta = bbeta; }
	void setChainLength(size_t min, size_t max) { MinChainLength = min; MaxChainLength = max; }

	// Optimization Contiguration Methods
	void setMaxIterations(size_t MaxIter) { options.MaxIter = MaxIter; }
	void setTolerance(double Tol) { options.Tol = Tol; }
	void setDisplay(bool Display) { options.Display = Display; }
	void setStepSize(double Step) { options.Step = Step; }
	void setBounds(double lower, double upper) { options.LowerBound = lower; options.UpperBound = upper; }

  // Internal TMCMC Methods
	void processChains();
	void saveResults();
  void prepareChains();
  void updateDatabase(double* point, double fitness);
  void processChainLink(size_t c);
  void calculate_statistics(double flc[], unsigned int sel[]);
  bool generateCandidate(int c);
  void precompute_chain_covariances(double** chain_cov, int newchains);
  int fmincon(const double *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt);
  int fminsearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt);
  int fzerofind(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt);
  static double tmcmc_objlogp(double x, const double *fj, int fn, double pj, double zero);
  static double tmcmc_objlogp_gsl(double x, void *param);
  static double tmcmc_objlogp_gsl2(const gsl_vector *v, void *param);
  static int compar_desc(const void *p1, const void *p2);  double compute_sum(double *v, int n);
  bool in_rect(double *v1, double *v2, double *diam, double sc, int D);
};

} // namespace Korali

extern Korali::KoraliTMCMC* _kt;

#endif // _TMCMC_H_
