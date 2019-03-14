#ifndef _TMCMC_H_
#define _TMCMC_H_

#include "solver.h"

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


class TMCMC : public Solver
{
  public:

  // TMCMC Configuration
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

	void Korali_InitializeInternalVariables();
  void runEngine();

	// TMCMC Configuration Methods
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
	void saveResults();
  void prepareGeneration();
  void updateDatabase(double* point, double fitness);
  void processSample(size_t c, double fitness);
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

#endif // _TMCMC_H_
