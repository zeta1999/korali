#include "tmcmc.h"
#include <numeric>
#include <chrono>

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multimin.h>

Korali::KoraliTMCMC* _kt;

Korali::KoraliTMCMC::KoraliTMCMC(Problem* problem, MPI_Comm comm) //: Korali::KoraliBase::KoraliBase(problem, comm)
{
  _problem = problem;
	_comm = comm;

	_popSize = 1000;
	_rankId = 0;
	_rankCount = 1;

  _bcastFuture = upcxx::make_future();
  _continueEvaluations = true;

	MaxStages = 20;

	MinChainLength = 1;
	MaxChainLength = 5;

	TolCOV  = 1;
	MinStep = 1e-9;
	bbeta   = 0.005;
	use_local_cov = false;

	options.MaxIter    = 1000;
	options.Tol        = 1e-12;
	options.Display    = false;
	options.Step       = 1e-8;
	options.LowerBound = -10.0;
	options.UpperBound = 10.0;

	range = gsl_rng_alloc (gsl_rng_default);
	gsl_rng_set(range, _problem->_seed+0xFFF0);
}

void Korali::KoraliTMCMC::run()
{
	_kt = this;
//	upcxx::init();
//	_rankId = upcxx::rank_me();
//	_rankCount = upcxx::rank_n();

	N = _problem->_parameterCount;

  // Verifying Parameter correctness.
	char errorString[500];
  if(Korali_VerifyParameters(errorString)) { if (_rankId == 0) fprintf(stderr, "%s", errorString); exit(-1); }

  if (_rankId == 0) Korali_SupervisorThread(); else Korali_WorkerThread();
//
//	upcxx::barrier();
//  upcxx::finalize();
}

void Korali::KoraliTMCMC::Korali_SupervisorThread()
{
	Korali_InitializeInternalVariables();

	for (int i = 0; i < _popSize; i++)
	{
		clFitness[i]  = _problem->evaluateFitness(&chainPoints[i*N]);
		clLogPrior[i] = _problem->getPriorsLogProbabilityDensity(&chainPoints[i*N]);

		for (int j = 0; j < N; j++) databasePoints[databaseEntries*N + j] = chainPoints[i*N + j];
		databaseFitness[databaseEntries] = clFitness[i];
		databaseEntries++;
	}

	printf("[Korali] TMCMC - Parameters: %ld, Seed: %ld\n", N, _problem->_seed) ;

  while(runinfo.p < 1.0 && ++runinfo.Gen < MaxStages) {
 	  auto gt0 = std::chrono::system_clock::now();
 	  processGeneration();
    auto gt1 = std::chrono::system_clock::now();
    printf("[Korali] Gen %d - Elapsed Time: %f, Annealing: %.2f%%\n", runinfo.Gen, std::chrono::duration<double>(gt1-gt0).count(), runinfo.p*100);
  }

	 saveResults();
}

void Korali::KoraliTMCMC::processGeneration()
{
	prepareNewGeneration();

	for (int c = 0; c < nChains; c++)
	{
		for (int step = 0; step < chainLength[c]; step++)
		{

			double* covariance = use_local_cov ? local_cov[c] : runinfo.SS;

	    gsl_matrix_view sigma_view 	= gsl_matrix_view_array(covariance, N,N);
	    gsl_vector_view mean_view 	= gsl_vector_view_array(&clPoints[c*N], N);
	    gsl_vector_view out_view  	= gsl_vector_view_array(&ccPoints[c*N], N);
		  gsl_ran_multivariate_gaussian(range, &mean_view.vector, &sigma_view.matrix, &out_view.vector);

			bool goodCandidate = true;
			for (int i = 0; i < N; i++)
			 if (ccPoints[c*N + i] < _problem->_parameters[i]._lowerBound ||
					 ccPoints[c*N + i] > _problem->_parameters[i]._upperBound) { goodCandidate = false; break; }

			if(goodCandidate)
			{
				ccFitness[c]  = _problem->evaluateFitness(&ccPoints[c*N]);
				ccLogPrior[c] = _problem->getPriorsLogProbabilityDensity(&ccPoints[c*N]);

				double L = exp((ccLogPrior[c]-clLogPrior[c])+(ccFitness[c]-clFitness[c])*runinfo.p);
				double P = gsl_ran_flat(range, 0.0, 1.0 );

				if (P < L) {
						for (int i = 0; i < N; ++i) clPoints[c*N + i] = ccPoints[c*N + i];
						clFitness[c]  = ccFitness[c];
						clLogPrior[c] = ccLogPrior[c];
				}
			}

			for (int i = 0; i < N; i++) databasePoints[databaseEntries*N + i] = clPoints[c*N + i]; 			// Re-add burn-in
			databaseFitness[databaseEntries] = clFitness[c];
			databaseEntries++;
		}
	}
}

void Korali::KoraliTMCMC::saveResults()
{
	double checksum = 0.0;

	for (int pos = 0; pos < databaseEntries; pos++)
		{
			for (int i = 0; i < N; i++) checksum += databasePoints[pos*N + i];
			checksum += databaseFitness[pos];
		}

	printf("[Korali] Checksum: %.20f\n", checksum);

	char* outputName = "tmcmc.txt";
	printf("[Korali] Saving results to file: %s.\n", outputName);
	FILE *fp = fopen(outputName, "w");
	for (int pos = 0; pos < databaseEntries; pos++)
	{
		for (int i = 0; i < N; i++) fprintf(fp, "%3.12lf, ", databasePoints[pos*N + i]);
		fprintf(fp, "%3.12lf\n", databaseFitness[pos]);
	}

	fclose(fp);
}

bool Korali::KoraliTMCMC::Korali_VerifyParameters(char* errorString)
{
	if (_problem->evaluateSettings(errorString)) return true;

	for (int i = 0; i < N; i++)
	if (_problem->_parameters[i]._boundsSet == false)
	{
		sprintf(errorString, "[Korali] Error: Bounds for parameter \'%s\' have not been set.\n", _problem->_parameters[i]._name.c_str());
		return true;
	}

  for (int i = 0; i < N; i++)
	if (_problem->_parameters[i]._priorSet == false)
	{
		sprintf(errorString, "[Korali] Error: Prior for parameter \'%s\' have not been set.\n", _problem->_parameters[i]._name.c_str());
		return true;
	}

  if(_popSize < 32 )
  {
  	sprintf(errorString, "[Korali] Error: Population Size (%lu) should be higher than 32.\n", _popSize);
  	return true;
  }

  return false;
}

void Korali::KoraliTMCMC::Korali_WorkerThread()
{

}

void Korali::KoraliTMCMC::Korali_InitializeInternalVariables()
{
  // Initialize Parameter Priors
  for (int i = 0; i < N; i++)
  	_problem->_parameters[i].initializePriorDistribution(_problem->_seed+i+1);

	// Initializing Data Variables
  double *LCmem  = (double*) calloc (_popSize*N*N, sizeof(double));
  local_cov = (double**) calloc ( _popSize, sizeof(double*));
  for (int pos=0; pos < _popSize; ++pos)
  {
  	local_cov[pos] = LCmem + pos*N*N;
    for (int i=0; i<N; ++i) local_cov[pos][i*N+i] = 1;
  }

  // Initializing Run Variables
  runinfo.CoefVar        = 0;
	runinfo.p              = 0;
	runinfo.currentuniques = 0;
	runinfo.logselections  = 0;
	runinfo.acceptance     = 0;
	runinfo.Gen = 0;
	runinfo.CoefVar = std::numeric_limits<double>::infinity();
	runinfo.SS =  (double*) calloc (N*N, sizeof(double));
	runinfo.meantheta =  (double*) calloc (MaxStages+1, sizeof(double));

	// Initializing TMCMC Leaders

//	chainPointsGlobalPtr  = upcxx::new_array<double>(N*_popSize);
//	chainLeaderFitnessGlobalPtr = upcxx::new_array<double>(_popSize);

	chainPoints = (double*) calloc (N*_popSize, sizeof(double)); //chainPointsGlobalPtr.local();
	clPoints    = (double*) calloc (N*_popSize, sizeof(double)); //chainPointsGlobalPtr.local();
	ccPoints    = (double*) calloc (N*_popSize, sizeof(double)); //chainPointsGlobalPtr.local();
	ccFitness   = (double*) calloc (_popSize, sizeof(double)); //chainLeaderFitnessGlobalPtr.local();
	ccLogPrior  = (double*) calloc (_popSize, sizeof(double)); //chainLeaderFitnessGlobalPtr.local();
	clFitness   = (double*) calloc (_popSize, sizeof(double)); //chainLeaderFitnessGlobalPtr.local();
	clLogPrior  = (double*) calloc (_popSize, sizeof(double));
	chainLength = (size_t*) calloc (_popSize, sizeof(size_t));

	databaseEntries = 0;
	databasePoints   = (double*) calloc (N*_popSize, sizeof(double));
  databaseFitness  = (double*) calloc (_popSize, sizeof(double));

  // First definition of chains and their leaders
  nChains = _popSize;
  for (int i = 0; i < _popSize; i++) for (int d = 0; d < N; d++)	chainPoints[i*N + d] = _problem->_parameters[d].getRandomNumber();

	// TODO: Ensure proper memory deallocation
}

void Korali::KoraliTMCMC::prepareNewGeneration()
{
	int n = databaseEntries;

	sort_t* list = (sort_t*) calloc (n, sizeof(sort_t));
	unsigned int *sel = (unsigned int*) calloc (n, sizeof(unsigned int));
	double **u = (double**) calloc (N, sizeof(double*));
	for (int i = 0; i < N; ++i) u[i] = (double*) calloc (n, sizeof(double));
	double *fj = (double*) calloc (n, sizeof(double));
	double *uf = (double*) calloc (n, sizeof(double));

	/* calculate u & acceptance rate */
	int un = 0, unflag;

	uf[un] = databaseFitness[0];
	for(int p = 0; p < N; ++p )
			u[p][un] = databasePoints[0*N + p];

	un++;
	for (int i = 1; i < n; ++i) {
			double xi[N];
			double fi = databaseFitness[i];
			for (int p = 0; p < N; ++p) xi[p] = databasePoints[i*N + p];

			unflag = 1;                 /* is this point unique? */
			for (int j = 0; j < un; ++j) {  /* compare with  previous u */
					for (int p = 0; p < N; ++p) {

							/* do they differ in position? */
							if (fabs(xi[p]-u[p][j]) > 1e-8) break; /* check next */

							/* do they differ in fun eval? */
							if (fabs(fi - uf[j]) > 1e-8) break; /* check next */

							unflag = 0;         /* not unique */
					}

					if (unflag == 0) break; /* not unique - stop comparison */
			}

			if (unflag) {               /* unique, put it in the table */
					uf[un] = fi;
					for (int p = 0; p < N; ++p) u[p][un] = xi[p];
					un++;
			}
	}
	runinfo.currentuniques = un;
	runinfo.acceptance     = (1.0*runinfo.currentuniques)/_popSize; /* check this*/

	for (int i = 0; i < n; ++i)
			fj[i] = databaseFitness[i];    /* separate point from F ?*/
	calculate_statistics(fj, sel);

	int newchains = 0;

	for (int i = 0; i < n; ++i) {
			list[i].idx  = i;
			list[i].nsteps = sel[i];
			list[i].F    = databaseFitness[i];
			if (sel[i] != 0) newchains++;
	}

	qsort(list, n, sizeof(sort_t), Korali::KoraliTMCMC::compar_desc);

	/* UPPER THRESHOLD */
	/* splitting long chains */
	//TODO: untested feature (DW)
	if (MaxChainLength > 0) {
			int initial_newchains = newchains;
			int h_threshold = MaxChainLength;
			for (int i = 0; i < initial_newchains; ++i) {
					if (list[i].nsteps > h_threshold) {
							while (list[i].nsteps > h_threshold) {
									list[newchains] = list[i];
									list[newchains].nsteps = h_threshold;
									list[i].nsteps = list[i].nsteps - h_threshold;
									newchains++;
							}
					}
			}
			qsort(list, n, sizeof(sort_t), Korali::KoraliTMCMC::compar_desc);
	}

	/* LOWER THRESHOLD */
	/* setting min chain length */
	//TODO: untested feature (DW)
	if (MinChainLength > 0) {
			int l_threshold = MinChainLength;
			for (int i = 0; i < newchains; ++i) {
					if ((list[i].nsteps > 0)&&(list[i].nsteps < l_threshold)) {
							list[i].nsteps = l_threshold;
					}
			}

			qsort(list, n, sizeof(sort_t), Korali::KoraliTMCMC::compar_desc);
	}

	int ldi = 0;                    /* leader index */
	for (int i = 0; i < n; ++i) {       /* newleader */
			if (list[i].nsteps != 0) {
					int idx = list[i].idx;
					for (int p = 0; p < N ; p++) {
							chainPoints[ldi*N + p] = databasePoints[idx*N + p];
					}
					clFitness[ldi] = databaseFitness[idx];
					chainLength[ldi] = list[i].nsteps;
					ldi++;
			}
	}

			for (int i = 0; i < newchains; ++i) {
					for (int p = 0; p < N; p++) {
							u[p][i] = chainPoints[i*N + p];
					}
			}

			double meanx[N], stdx[N];
			for (int p = 0; p < N; p++) {
					meanx[p] = gsl_stats_mean(u[p], 1, newchains);
					stdx[p]  = gsl_stats_sd_m(u[p], 1, newchains, meanx[p]);
			}

			if(options.Display) {
				printf("prepare_newgen: CURGEN DB (LEADER) %d: [nlead=%d]\n", runinfo.Gen, newchains);
					//print_matrix("means", meanx, N);
					//print_matrix("std", stdx, N);
			}

	if (use_local_cov) precompute_chain_covariances(local_cov, newchains);

	databaseEntries = 0;
	nChains = newchains;
	for (int c = 0; c < nChains; c++) for (int i = 0; i < N; i++) clPoints[c*N + i] = chainPoints[c*N + i];

	for (int i = 0; i < N; ++i) free(u[i]);
	free(u);
	free(sel);
	free(list);
	free(fj);
	free(uf);
}

void Korali::KoraliTMCMC::calculate_statistics(double flc[], unsigned int sel[])
{

	double *flcp  = (double*) calloc (databaseEntries, sizeof(double));
	double *weight     = (double*) calloc (databaseEntries, sizeof(double));
	double *q = (double*) calloc (databaseEntries, sizeof(double));
	unsigned int *nn = (unsigned int*) calloc (databaseEntries, sizeof(unsigned int));

    int display = options.Display;
    double coefVar       = runinfo.CoefVar;
    double logselections = runinfo.logselections;

    double fmin = 0, xmin = 0;
    bool conv = 0;

    bool useFminCon = false;
    bool useFminSearch = true;
    bool useFminZeroFind = false;

    if (useFminCon)                 conv = fmincon(flc, databaseEntries, runinfo.p, TolCOV, &xmin, &fmin, options);
    if (useFminSearch)   if (!conv) conv = fminsearch(flc, databaseEntries, runinfo.p, TolCOV, &xmin, &fmin, options);
    if (useFminZeroFind) if (!conv) conv = fzerofind(flc, databaseEntries, runinfo.p, TolCOV, &xmin, &fmin, options);

    double pPrev = runinfo.p;

    if ( conv && (xmin > pPrev) ) {
    	runinfo.p       = xmin;
        coefVar = pow(fmin, 0.5) + TolCOV;
    } else {
    	runinfo.p       = pPrev + MinStep;
        coefVar = pow(tmcmc_objlogp(runinfo.p, flc, databaseEntries, pPrev, TolCOV), 0.5) + TolCOV;
    }

    if (runinfo.p > 1.0) {
    	runinfo.p    = 1.0;
        coefVar = pow(tmcmc_objlogp(runinfo.p, flc, databaseEntries,  pPrev, TolCOV), 0.5) +   TolCOV;
    }

    /* Compute weights and normalize*/

    for (int i = 0; i < databaseEntries; i++)
        flcp[i] = flc[i]*(runinfo.p-pPrev);

    const double fjmax = gsl_stats_max(flcp, 1, databaseEntries);

    for (int i = 0; i < databaseEntries; i++)
        weight[i] = exp( flcp[i] - fjmax );


    double sum_weight = std::accumulate(weight, weight+databaseEntries, 0.0);
    logselections  = log(sum_weight) + fjmax - log(databaseEntries);
    if (display) printf("logselections: %f\n", logselections);


    for (int i = 0; i < databaseEntries; i++) q[i] = weight[i]/sum_weight;


    if (display) printf("CoefVar: %f\n", coefVar);



    for (int i = 0; i < databaseEntries; i++) sel[i] = 0;

    gsl_ran_multinomial(range, databaseEntries, _popSize, q, nn);
    for (int i = 0; i < databaseEntries; ++i) sel[i]+=nn[i];


    for (int i = 0; i < N; i++)
    {
    	runinfo.meantheta[i] = 0;
      for (int j = 0; j < databaseEntries; j++) runinfo.meantheta[i] += databasePoints[j*N+ i]*q[j];
    }

    double meanv[N];
    for (int i = 0; i < N; i++)  meanv[i] = runinfo.meantheta[i];
    for (int i = 0; i < N; i++) for (int j = i; j < N; ++j)
    {
			double s = 0.0;
			for (unsigned int k = 0; k < databaseEntries; ++k) s += q[k]*(databasePoints[k*N+i]-meanv[i])*(databasePoints[k*N+j]-meanv[j]);
			runinfo.SS[i*N + j] = runinfo.SS[j*N + i] = s*bbeta;
    }

    gsl_matrix_view sigma 	= gsl_matrix_view_array(runinfo.SS, N,N);
    gsl_linalg_cholesky_decomp( &sigma.matrix );

  	free(flcp);
  	free(weight);
  	free(q);
  	free(nn);
}

void Korali::KoraliTMCMC::precompute_chain_covariances(double** chain_cov, int newchains)
{
    bool display = options.Display;
    printf("Precomputing chain covariances for the current generation...\n");

    // allocate space
    int* nn_ind        = (int*) calloc (newchains, sizeof(int));
    int* nn_count      = (int*) calloc (newchains, sizeof(int));
    double* diam       = (double*) calloc (N, sizeof(double));
    double* chain_mean = (double*) calloc (N, sizeof(double));
    gsl_matrix* work   = gsl_matrix_alloc(N, N);

    // find diameters
    for (int d = 0; d < N; ++d) {
        double d_min = +1e6;
        double d_max = -1e6;
        for (int pos = 0; pos < _popSize; ++pos) {
            double s = databasePoints[pos*N+d];
            if (d_min > s) d_min = s;
            if (d_max < s) d_max = s;
        }
        diam[d] = d_max-d_min;
        if (display) printf("Diameter %d: %.6lf\n", d, diam[d]);
    }

    int idx, pos;
    int status = 0;
    double ds = 0.05;
    for (double scale = 0.1; scale <= 1.0; scale += ds) {
        // find neighbors in a rectangle - O(_popSize^2)
        for (pos = 0; pos < newchains; ++pos) {
            nn_count[pos] = 0;
            double* curr = &chainPoints[pos*N];
            for (int i = 0; i < _popSize; ++i) {
                double* s = &databasePoints[i*N];
                if (in_rect(curr, s, diam, scale, N)) {
                    nn_ind[pos*_popSize+nn_count[pos]] = i;
                    nn_count[pos]++;
                }
            }
        }

        // compute the covariances
        for (pos = 0; pos < newchains; ++pos) {
            for (int d = 0; d < N; ++d) {
                chain_mean[d] = 0;
                for (int k = 0; k < nn_count[pos]; ++k) {
                    idx = nn_ind[pos*_popSize+k];
                    chain_mean[d] += databasePoints[idx*N+d];
                }
                chain_mean[d] /= nn_count[pos];
            }

            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j) {
                    double s = 0;
                    for (int k = 0; k < nn_count[pos]; k++) {
                        idx = nn_ind[pos*_popSize+k];
                        s  += (databasePoints[idx*N+i]-chain_mean[i]) *
                              (databasePoints[idx*N+j]-chain_mean[j]);
                    }
                    chain_cov[pos][i*N+j] = chain_cov[pos][j*N+i] = s/nn_count[pos];
                }

            // check if the matrix is positive definite
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j) {
                    double s = chain_cov[pos][i*N+j];
                    gsl_matrix_set(work, i, j, s);
                }
            gsl_set_error_handler_off();
            status = gsl_linalg_cholesky_decomp(work);
            if (status == GSL_SUCCESS) break;
        }
    }

    for (pos = 0; pos < newchains; ++pos) {
      gsl_matrix_view sigma 	= gsl_matrix_view_array(chain_cov[pos], N,N);
      gsl_linalg_cholesky_decomp( &sigma.matrix );
    }

    if (status != GSL_SUCCESS) {
    	fprintf(stderr, "[Korali] Error: GSL failed to create Chain Covariance Matrix.\n");
    }



    // deallocate space
    free(nn_ind);
    free(nn_count);
    free(diam);
    free(chain_mean);
    gsl_matrix_free(work);
}

double Korali::KoraliTMCMC::tmcmc_objlogp(double x, const double *fj, int fn, double pj, double zero)
{

  double *weight = (double*) calloc (fn, sizeof(double));
  double *q      = (double*) calloc (fn, sizeof(double));

    const double fjmax = gsl_stats_max(fj, 1, fn);

    for(int i = 0; i <fn; ++i)
        weight[i] = exp((fj[i]-fjmax)*(x-pj));

    double sum_weight = std::accumulate(weight, weight+fn, 0.0);



    for(int i = 0; i < fn; ++i) {
        q[i] = weight[i]/sum_weight;
    }

    double mean_q = gsl_stats_mean(q, 1, fn);
    double std_q  = gsl_stats_sd_m(q, 1, fn, mean_q);
    double cov2   = pow(std_q/mean_q-zero, 2);

    free(weight);
    free(q);

    return cov2;
}


double Korali::KoraliTMCMC::tmcmc_objlogp_gsl(double x, void *param)
{
    fparam_t *fp = (fparam_t *) param;
    return Korali::KoraliTMCMC::tmcmc_objlogp(x, fp->fj, fp->fn, fp->pj, fp->tol);
}


double Korali::KoraliTMCMC::tmcmc_objlogp_gsl2(const gsl_vector *v, void *param)
{
    double x = gsl_vector_get(v, 0);
    return Korali::KoraliTMCMC::tmcmc_objlogp_gsl(x, param);
}


int Korali::KoraliTMCMC::fmincon(const double *fj, int fn, double pj, double objTol,  double *xmin, double *fmin, const optim_options& opt)
{

    int status;
    size_t iter     = 0;
    size_t max_iter = opt.MaxIter;
    double tol      = opt.Tol;
    bool display    = opt.Display;
    double x_lo     = opt.LowerBound;
    double x_hi     = opt.UpperBound;

    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;
    gsl_function F;
    gsl_vector *x;

    double m  = 0.5*(x_hi-x_lo);
    double fm = 0.0;

    bool converged = false;

    x = gsl_vector_alloc (1);

    fparam_t fp;
    fp.fj = fj;
    fp.fn = fn;
    fp.pj = pj;
    fp.tol = objTol;

    F.function = Korali::KoraliTMCMC::tmcmc_objlogp_gsl;
    F.params = &fp;

    // SELECT ONE MINIMIZER STRATEGY
    T = gsl_min_fminimizer_brent;
    /*	T = gsl_min_fminimizer_goldensection;*/
    /*	T = gsl_min_fminimizer_quad_golden;; */
    s = gsl_min_fminimizer_alloc (T);

    double f_lo = tmcmc_objlogp_gsl(x_lo, &fp);
    double f_hi = tmcmc_objlogp_gsl(x_hi, &fp);
    if (f_lo < f_hi) {
        m  = x_lo;
        fm = f_lo;
    } else {
        m  = x_hi;
        fm = f_hi;
    }


    for (int i = 1; i < max_iter; ++i) {
        double x = x_lo + i*(x_hi-x_lo)/max_iter;
        double fx = tmcmc_objlogp_gsl(x, &fp);
        if (fx < fm) {
            m  = x;
            fm = fx;
        }
    }

    if (fm <= tol) {
        converged = true;
        gsl_vector_free(x);
        gsl_min_fminimizer_free (s);
        return converged;
    }

    if ((fm < f_lo) && (fm < f_hi)) {
    } else {
        return 0;
    }

    gsl_min_fminimizer_set (s, &F, m, x_lo, x_hi);

    if (display) {
        printf ("fmincon: Using %s method\n", gsl_min_fminimizer_name (s));
        printf ("%5s [%18s, %18s] %18s %18s %18s\n", "iter", "lower", "upper", "min", "fmin", "err(est)");
        printf ("%5zu [%.16f, %.16f] %.16f %.16f %.16f\n", iter, x_lo, x_hi, m, fm, x_hi - x_lo);
    }

    do {
        iter++;
        status = gsl_min_fminimizer_iterate (s);

        m    = gsl_min_fminimizer_x_minimum (s);
        x_lo = gsl_min_fminimizer_x_lower (s);
        x_hi = gsl_min_fminimizer_x_upper (s);

        status = gsl_min_test_interval (x_lo, x_hi, tol, tol);
        if (status == GSL_SUCCESS) {
            if (gsl_min_fminimizer_f_minimum(s) <= tol) {
                if (display) printf ("fmincon: Converged:\n");
                converged = true;
            } else {
                if (display) printf ("fmincon: Converged but did not find minimum:\n");
            }
        } else if (gsl_min_fminimizer_f_minimum(s) <= tol) {
            converged = true;
            status = GSL_SUCCESS;
            if (display) printf ("fmincon: Did not converge but found minimum at\n");
        }

        if (display)
            printf ("%5zu [%.16f, %.16f] %.16f f()=%.16f %.16f\n",
                    iter, x_lo, x_hi, m, gsl_min_fminimizer_f_minimum(s), x_hi - x_lo);

    } while (status == GSL_CONTINUE && iter < max_iter);

    if (converged) {
        converged = 1;
        gsl_vector_set (x, 0, m);
        *fmin = tmcmc_objlogp_gsl(m, &fp);
        *xmin = m;
    } else {
        *fmin = 0;
        *xmin = 0.0;
    }

    gsl_vector_free(x);
    gsl_min_fminimizer_free (s);

    return converged;
}


int Korali::KoraliTMCMC::fminsearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt)
{
    const gsl_multimin_fminimizer_type *T;
    gsl_multimin_fminimizer *s = NULL;
    gsl_vector *ss, *x;
    gsl_multimin_function minex_func;
    bool converged = 0;

    size_t iter     = 0;
    size_t max_iter = opt.MaxIter;
    double tol      = opt.Tol;
    bool display    = opt.Display;
    double step     = opt.Step;

    int status;
    double size;

    fparam_t fp;
    fp.fj = fj;
    fp.fn = fn;
    fp.pj = pj;
    fp.tol = objTol;

    x = gsl_vector_alloc (1);
    gsl_vector_set (x, 0, pj);

    ss = gsl_vector_alloc (1);
    gsl_vector_set_all (ss, step);

    minex_func.n      = 1;
    minex_func.f      = tmcmc_objlogp_gsl2;
    minex_func.params = &fp;

    // SELECT ONE MINIMIZER STRATEGY
    /*	T = gsl_multimin_fminimizer_nmsimplex;*/
    T = gsl_multimin_fminimizer_nmsimplex2;
    /*	T = gsl_multimin_fminimizer_nmsimplex2rand;*/
    s = gsl_multimin_fminimizer_alloc (T, 1);
    gsl_multimin_fminimizer_set (s, &minex_func, x, ss);

    if (display) printf ("fminsearch: Using %s method\n", gsl_multimin_fminimizer_name (s));

    do {
        iter++;
        status = gsl_multimin_fminimizer_iterate(s);

        if (status) break;

        size   = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, tol);

        if (status == GSL_SUCCESS) {
            if(s->fval <= tol) {
                converged = true;
                if (display) printf ("fminsearch: Converged to minimum at\n");
            } else {
                converged = false;
                if (display) printf ("fminsearch: Converged but did not find minimum.\n");
            }
        } else if (s->fval <= tol) {
            converged = true;
            status = GSL_SUCCESS;
            if (display)
                printf ("fminsearch: NOT converged but found minimum at\n");
        } else {
            converged = false;
            if (display)
                printf("fminsearch: NOT converged and did not find minimum.\n");
        }
        if (display) printf ("%3ld x =  %.16lf f() = %.16f size = %.16f\n",
                                 iter, gsl_vector_get (s->x, 0), s->fval, size);

    } while (status == GSL_CONTINUE && iter < max_iter);

    if (converged) {
        *fmin = s->fval;
        *xmin = gsl_vector_get(s->x, 0);
    } else {
        *fmin = 0;
        *xmin = 0.0;
    }

    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free (s);

    return converged;
}

/* simple min search, check stepwise for x < opt.Tol; if unsuccessfull
 * adjust range and refine steps (DW: could be optimized) */
int Korali::KoraliTMCMC::fzerofind(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin, const optim_options& opt)
{
    bool display = opt.Display;
    double tol   = opt.Tol;
    double step  = opt.Step;
    double x_lo  = opt.LowerBound;
    double x_hi  = opt.UpperBound;

    int first_try = true;

    size_t iter;
    size_t niters;

    double min  = 0;
    double fm = std::numeric_limits<double>::max();

    bool converged = false;

    while (converged == false && 1e-16 < step) {

        if(display) printf("fzerofind: x_lo %e x_hi %ei step %e\n", x_lo, x_hi, step);
        niters = (size_t) ((x_hi-x_lo)/step);

        if (first_try)  first_try = false;

        for (iter = 0; iter < niters; ++iter) {
            double x  = x_lo + iter*step;
            double fx = tmcmc_objlogp(x, fj, fn, pj, objTol);

            if (fx < fm) {
                fm  = fx;
                min = x;
            }
            if (fx <= tol) {
                converged = true;
                break;
            }
        }

        if (converged) {
            if (display) printf("fzerofind converged: m=%.16f fm=%.16f iter=%ld\n", min, fm, niters);
        } else {
            x_lo = min - 10*step;
            if (x_lo < 0) x_lo = opt.LowerBound;
            x_hi = min + 10*step;
            if (x_hi > 4) x_hi = opt.UpperBound;
            step *= 0.1;
            if (display) printf("fzerofind (%e): m=%.16f fm=%.16f iter=%ld\n",
                                    step, min, fm, niters);
        }

    }

    *xmin = min;
    *fmin = fm;

    return converged;
}

bool Korali::KoraliTMCMC::in_rect(double *v1, double *v2, double *diam, double sc, int D)
{
    for (int d = 0; d < D; d++)  if (fabs(v1[d]-v2[d]) > sc*diam[d]) return false;
    return true;
}


int Korali::KoraliTMCMC::compar_desc(const void* p1, const void* p2)
{
    int dir = +1;   /* -1: ascending order, +1: descending order */
    sort_t *s1 = (sort_t *) p1;
    sort_t *s2 = (sort_t *) p2;

    if (s1->nsteps < s2->nsteps) return dir;
    if (s1->nsteps > s2->nsteps) return -dir;

    return 0;
}
