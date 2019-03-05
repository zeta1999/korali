#ifndef _TMCMC_H_
#define _TMCMC_H_

#include "tmcmc_utils.h"
#include "mpi.h"
#include <upcxx/upcxx.hpp>
#include "problem.h"
#include <queue>

namespace Korali
{


class KoraliTMCMC // : public KoraliBase
{
  public:

	// Korali Runtime Variables
  MPI_Comm _comm;
  int _rankId;
  int _rankCount;

  Problem* _problem;
  double* _fitnessVector;
  bool _continueEvaluations;

  std::queue<int> _workers;
  upcxx::future<> _bcastFuture;

  size_t _popSize; // Number of offspring per sample cycle

	// TMCMC Fields
  data_t    data;
  runinfo_t runinfo;
  cgdbp_t *leaders;

  db_t    full_db;
  cgdb_t  curgen_db;
  resdb_t curres_db;

	gsl_rng  *range;

  // Public Methods

  // Korali Engine Methods
	KoraliTMCMC(Problem* problem, MPI_Comm comm);
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
	void setMaxStages(int MaxStages) { data.MaxStages = MaxStages; }
	void setToleranceCOV(double TolCOV) { data.TolCOV = TolCOV; }
	void setUseLocalCOV(double use_local_cov) { data.use_local_cov = use_local_cov; }
	void setCovarianceScaling(double bbeta) { data.bbeta = bbeta; }
	void setBurnInIterations(int burn_in) { data.burn_in = burn_in; }
	void setChainLength(size_t min, size_t max) { data.MinChainLength = min; data.MaxChainLength = max; }

	// Optimization Contiguration Methods
	void setMaxIterations(size_t MaxIter) { data.options.MaxIter = MaxIter; }
	void setTolerance(double Tol) { data.options.Tol = Tol; }
	void setDisplay(bool Display) { data.options.Display = Display; }
	void setStepSize(double Step) { data.options.Step = Step; }
	void setBounds(double lower, double upper) { data.options.LowerBound = lower; data.options.UpperBound = upper; }

  // Internal TMCMC Methods
  int prepareNewGeneration(int nchains, cgdbp_t *leaders);
  void update_curgen_db(double point[], double F, double prior);
  void calculate_statistics(double flc[], int nselections, int gen, unsigned int sel[]);
  void precompute_chain_covariances(const cgdbp_t* leader,double** init_mean, double** chain_cov, int newchains);
  int fmincon(const double *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt);
  int fminsearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt);
  int fzerofind(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt);
  static double tmcmc_objlogp(double x, const double *fj, int fn, double pj, double zero);
  static double tmcmc_objlogp_gsl(double x, void *param);
  static double tmcmc_objlogp_gsl2(const gsl_vector *v, void *param);

};

} // namespace Korali

extern Korali::KoraliTMCMC* _kt;

#endif // _TMCMC_H_
