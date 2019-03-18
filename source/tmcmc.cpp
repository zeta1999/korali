#include "tmcmc.h"
#include <numeric>
#include <limits>
#include <chrono>

#include <gsl/gsl_eigen.h>
#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multimin.h>

Korali::TMCMC::TMCMC(Problem* problem, MPI_Comm comm) : Korali::Solver::Solver(problem, comm)
{
	MinChainLength = 1;
	MaxChainLength = 100;

	TolCOV  = 1;
	MinStep = 1e-9;
	bbeta   = 0.005;
	use_local_cov = false;

	options.MaxIter    = 100;
	options.Tol        = 1e-12;
	options.Display    = false;
	options.Step       = 1e-8;
	options.LowerBound = -10.0;
	options.UpperBound = 10.0;
}

void Korali::TMCMC::runEngine()
{
	printf("[Korali] TMCMC - Parameters: %ld, Seed: %ld\n", N, _problem->_seed);

	Korali_InitializeInternalVariables();

	auto startTime = std::chrono::system_clock::now();

  while(_continueEvaluations) {

  	auto t0 = std::chrono::system_clock::now();

 		while (finishedChains < nChains)
 		{
 			for (int c = 0; c < nChains; c++) if (chainCurrentStep[c] < chainLength[c]) if (chainPendingFitness[c] == false)
 			{
 				chainPendingFitness[c] = true;
 				if(generateCandidate(c)) evaluateSample(c);
 				else                     processSample(c, -DBL_MAX);
 			}
 			checkProgress();
 		}

    auto t1 = std::chrono::system_clock::now();

    printf("[Korali] Gen %d - Time: %fs, Annealing: %.2f%%, Acceptance: %.2f%%\n", runinfo.Gen, std::chrono::duration<double>(t1-t0).count(), runinfo.p*100, runinfo.acceptanceRate*100);
    runinfo.Gen++;
 	  _continueEvaluations = runinfo.p < 1.0 && runinfo.Gen < _maxGens;

 	 auto t2 = std::chrono::system_clock::now();
 	  if (_continueEvaluations) prepareGeneration();
 	 auto t3 = std::chrono::system_clock::now();

   printf("[Korali] Gen Time: %fs\n", std::chrono::duration<double>(t3-t2).count());
  }

  auto endTime = std::chrono::system_clock::now();

  printf("[Korali] Finished - Time: %fs\n", std::chrono::duration<double>(endTime-startTime).count());

	saveResults();
}

void Korali::TMCMC::processSample(size_t c, double fitness)
{
	ccFitness[c] = fitness;
	ccLogPrior[c] = _problem->getPriorsLogProbabilityDensity(&ccPoints[c*N]);
	double L = exp((ccLogPrior[c]-clLogPrior[c])+(ccFitness[c]-clFitness[c])*runinfo.p);
	double P = gsl_ran_flat(chainGSLRange[c], 0.0, 1.0);

	if (P < L) {
			for (int i = 0; i < N; i++) clPoints[c*N + i] = ccPoints[c*N + i];
			clFitness[c]  = ccFitness[c];
			clLogPrior[c] = ccLogPrior[c];
			runinfo.uniqueEntries++;
	}

	updateDatabase(&clPoints[c*N], clFitness[c]);
	chainCurrentStep[c]++;
	chainPendingFitness[c] = false;
	if (chainCurrentStep[c] == chainLength[c]) finishedChains++;
}

void Korali::TMCMC::updateDatabase(double* point, double fitness)
{
	for (int i = 0; i < N; i++) databasePoints[databaseEntries*N + i] = point[i]; 			// Re-add burn-in
	databaseFitness[databaseEntries] = fitness;
	databaseEntries++;
}

bool Korali::TMCMC::generateCandidate(int c)
{
	if (runinfo.Gen == 0) return true;

	double* covariance = use_local_cov ? local_cov[c] : runinfo.SS;
  gsl_vector_view out_view	= gsl_vector_view_array(&ccPoints[c*N], N);
  gsl_matrix_view sigma_view 	= gsl_matrix_view_array(covariance, N,N);
  gsl_vector_view mean_view 	= gsl_vector_view_array(&clPoints[c*N], N);
	gsl_ran_multivariate_gaussian(chainGSLRange[c], &mean_view.vector, &sigma_view.matrix, &out_view.vector);

	for (int i = 0; i < N; i++)
	  if (ccPoints[c*N + i] < _problem->_parameters[i]._lowerBound || ccPoints[c*N + i] > _problem->_parameters[i]._upperBound) return false;

	return true;
}

void Korali::TMCMC::saveResults()
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

void Korali::TMCMC::Korali_InitializeInternalVariables()
{
	// Initializing Data Variables
  double *LCmem  = (double*) calloc (_sampleCount*N*N, sizeof(double));
  local_cov = (double**) calloc ( _sampleCount, sizeof(double*));
  for (int pos=0; pos < _sampleCount; ++pos)
  {
  	local_cov[pos] = LCmem + pos*N*N;
    for (int i=0; i<N; i++) local_cov[pos][i*N+i] = 1;
  }

  // Initializing Run Variables
  runinfo.CoefVar        = 0;
	runinfo.p              = 0;
	runinfo.uniqueSelections = 0;
	runinfo.uniqueEntries = _sampleCount;
	runinfo.logselections  = 0;
	runinfo.acceptanceRate     = 1.0;
	runinfo.Gen = 0;
	runinfo.CoefVar = std::numeric_limits<double>::infinity();
	runinfo.SS =  (double*) calloc (N*N, sizeof(double));
	runinfo.meantheta =  (double*) calloc (N+1, sizeof(double));

	// Initializing TMCMC Leaders
	ccPoints    = sampleGlobalPtr.local();
	ccFitness   = (double*) calloc (_sampleCount, sizeof(double));
	ccLogPrior  = (double*) calloc (_sampleCount, sizeof(double)); //chainLeaderFitnessGlobalPtr.local();

	clPoints    = (double*) calloc (N*_sampleCount, sizeof(double));
	clFitness   = (double*) calloc (_sampleCount, sizeof(double)); //chainLeaderFitnessGlobalPtr.local();
	clLogPrior  = (double*) calloc (_sampleCount, sizeof(double));

	chainPendingFitness = (bool*) calloc (_sampleCount, sizeof(bool));
	chainCurrentStep    = (size_t*) calloc (_sampleCount, sizeof(size_t));
	chainLength         = (size_t*) calloc (_sampleCount, sizeof(size_t));

	databaseEntries  = 0;
	databasePoints   = (double*) calloc (N*_sampleCount, sizeof(double));
  databaseFitness  = (double*) calloc (_sampleCount, sizeof(double));

  // First definition of chains and their leaders
  nChains = _sampleCount;
	finishedChains = 0;
  for (int c = 0; c < _sampleCount; c++) for (int d = 0; d < N; d++)  clPoints[c*N + d] = ccPoints[c*N + d] = _problem->_parameters[d].getRandomNumber();
  for (int c = 0; c < _sampleCount; c++) clLogPrior[c] = _problem->getPriorsLogProbabilityDensity(&clPoints[c*N]);
  for (int c = 0; c < _sampleCount; c++) chainCurrentStep[c] = 0;
  for (int c = 0; c < _sampleCount; c++) chainLength[c] = 1;
  for (int c = 0; c < _sampleCount; c++) chainPendingFitness[c] = false;

  // Setting Chain-Specific Seeds
  range = gsl_rng_alloc (gsl_rng_default);
  gsl_rng_set(range, _problem->_seed+N+0xD00);

  chainGSLRange = (gsl_rng**) calloc (_sampleCount, sizeof(gsl_rng*));
  for (int c = 0; c < _sampleCount; c++)
  {
  	chainGSLRange[c] = gsl_rng_alloc (gsl_rng_default);
  	gsl_rng_set(chainGSLRange[c], _problem->_seed+N+0xF00+c);
  }

	// TODO: Ensure proper memory deallocation
}

void Korali::TMCMC::prepareGeneration()
{
	sort_t* list = (sort_t*) calloc (databaseEntries, sizeof(sort_t));
	unsigned int *sel = (unsigned int*) calloc (databaseEntries, sizeof(unsigned int));

	int newchains = 0;
	resampleLeaders(sel);
	for (int i = 0; i < databaseEntries; i++) if (sel[i] != 0) newchains++;

	for (int i = 0; i < databaseEntries; i++) {
			list[i].idx  = i;
			list[i].nsteps = sel[i];
			list[i].F    = databaseFitness[i];
	}


	int ldi = 0;
	for (int i = 0; i < databaseEntries; i++) {
			if (sel[i] != 0) {
					for (int p = 0; p < N ; p++) clPoints[ldi*N + p] = databasePoints[i*N + p];
					clFitness[ldi] = databaseFitness[i];
					chainLength[ldi] = sel[i];
					ldi++;
			}
	}

	if (use_local_cov) precompute_chain_covariances(local_cov, newchains);

	databaseEntries = 0;
	runinfo.uniqueEntries = 0;
	nChains = newchains;
	finishedChains = 0;
	for (int c = 0; c < nChains; c++) chainCurrentStep[c] = 0;
	for (int c = 0; c < nChains; c++) chainPendingFitness[c] = false;

	free(sel);
	free(list);
}

void Korali::TMCMC::resampleLeaders(unsigned int sel[])
{

	double *flcp  = (double*) calloc (databaseEntries, sizeof(double));
	double *weight     = (double*) calloc (databaseEntries, sizeof(double));
	double *q = (double*) calloc (databaseEntries, sizeof(double));
	unsigned int *nn = (unsigned int*) calloc (databaseEntries, sizeof(unsigned int));

	double coefVar       = runinfo.CoefVar;
	double logselections = runinfo.logselections;

	double fmin = 0, xmin = 0;
  fminsearch(databaseFitness, databaseEntries, runinfo.p, TolCOV, &xmin, &fmin);

	double pPrev = runinfo.p;

	if (xmin > pPrev) {
		runinfo.p       = xmin;
		coefVar = sqrt(fmin) + TolCOV;
	} else {
		runinfo.p       = pPrev + MinStep;
		coefVar = sqrt(tmcmc_objlogp(runinfo.p, databaseFitness, databaseEntries, pPrev, TolCOV)) + TolCOV;
	}

	if (runinfo.p > 1.0) {
		runinfo.p    = 1.0;
		coefVar = sqrt(tmcmc_objlogp(runinfo.p, databaseFitness, databaseEntries,  pPrev, TolCOV)) + TolCOV;
	}

	/* Compute weights and normalize*/

	for (int i = 0; i < databaseEntries; i++) flcp[i] = databaseFitness[i]*(runinfo.p-pPrev);
	const double fjmax = gsl_stats_max(flcp, 1, databaseEntries);
	for (int i = 0; i < databaseEntries; i++)	weight[i] = exp( flcp[i] - fjmax );

	double sum_weight = std::accumulate(weight, weight+databaseEntries, 0.0);
	logselections  = log(sum_weight) + fjmax - log(databaseEntries);

	for (int i = 0; i < databaseEntries; i++) q[i] = weight[i]/sum_weight;

	for (int i = 0; i < databaseEntries; i++) sel[i] = 0;
	gsl_ran_multinomial(range, databaseEntries, _sampleCount, q, nn);
	for (int i = 0; i < databaseEntries; i++) sel[i]+=nn[i];

	int zeroCount = 0;
	for (int i = 0; i < databaseEntries; i++) if (sel[i] == 0) zeroCount++;
	runinfo.uniqueSelections = databaseEntries - zeroCount;
	runinfo.acceptanceRate     = (1.0*runinfo.uniqueSelections)/_sampleCount;

	for (int i = 0; i < N; i++)
	{
		runinfo.meantheta[i] = 0;
		for (int j = 0; j < databaseEntries; j++) runinfo.meantheta[i] += databasePoints[j*N + i]*q[j];
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

void Korali::TMCMC::precompute_chain_covariances(double** chain_cov, int newchains)
{
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
        for (int pos = 0; pos < _sampleCount; ++pos) {
            double s = databasePoints[pos*N+d];
            if (d_min > s) d_min = s;
            if (d_max < s) d_max = s;
        }
        diam[d] = d_max-d_min;
        if (options.Display) printf("Diameter %d: %.6lf\n", d, diam[d]);
    }

    int idx, pos;
    int status = 0;
    double ds = 0.05;
    for (double scale = 0.1; scale <= 1.0; scale += ds) {
        // find neighbors in a rectangle - O(_sampleCount^2)
        for (pos = 0; pos < newchains; ++pos) {
            nn_count[pos] = 0;
            double* curr = &clPoints[pos*N];
            for (int i = 0; i < _sampleCount; i++) {
                double* s = &databasePoints[i*N];
                bool isInRectangle = true;
 						    for (int d = 0; d < N; d++)  if (fabs(curr[d]-s[d]) > scale*diam[d]) isInRectangle = false;
                 if (isInRectangle) {
                     nn_ind[pos*_sampleCount+nn_count[pos]] = i;
                     nn_count[pos]++;
                 }
            }
        }

        // compute the covariances
        for (pos = 0; pos < newchains; ++pos) {
            for (int d = 0; d < N; ++d) {
                chain_mean[d] = 0;
                for (int k = 0; k < nn_count[pos]; ++k) {
                    idx = nn_ind[pos*_sampleCount+k];
                    chain_mean[d] += databasePoints[idx*N+d];
                }
                chain_mean[d] /= nn_count[pos];
            }

            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; ++j) {
                    double s = 0;
                    for (int k = 0; k < nn_count[pos]; k++) {
                        idx = nn_ind[pos*_sampleCount+k];
                        s  += (databasePoints[idx*N+i]-chain_mean[i]) *
                              (databasePoints[idx*N+j]-chain_mean[j]);
                    }
                    chain_cov[pos][i*N+j] = chain_cov[pos][j*N+i] = s/nn_count[pos];
                }

            // check if the matrix is positive definite
            for (int i = 0; i < N; i++)
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

double Korali::TMCMC::tmcmc_objlogp(double x, const double *fj, int fn, double pj, double zero)
{
  double *weight = (double*) calloc (fn, sizeof(double));
  double *q      = (double*) calloc (fn, sizeof(double));

    const double fjmax = gsl_stats_max(fj, 1, fn);

    for(int i = 0; i <fn; i++)weight[i] = exp((fj[i]-fjmax)*(x-pj));

    double sum_weight = std::accumulate(weight, weight+fn, 0.0);

    for(int i = 0; i < fn; i++)  q[i] = weight[i]/sum_weight;

    double mean_q = gsl_stats_mean(q, 1, fn);
    double std_q  = gsl_stats_sd_m(q, 1, fn, mean_q);
    double cov2   = (std_q/mean_q-zero); cov2 *= cov2;

    free(weight);
    free(q);

    return cov2;
}

double Korali::TMCMC::tmcmc_objlogp_gsl(const gsl_vector *v, void *param)
{
    double x = gsl_vector_get(v, 0);
    fparam_t *fp = (fparam_t *) param;
    return Korali::TMCMC::tmcmc_objlogp(x, fp->fj, fp->fn, fp->pj, fp->tol);
}

void Korali::TMCMC::fminsearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin)
{
    const gsl_multimin_fminimizer_type *T;
    gsl_multimin_fminimizer *s = NULL;
    gsl_vector *ss, *x;
    gsl_multimin_function minex_func;

    size_t iter     = 0;
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
    gsl_vector_set_all (ss, options.Step);

    minex_func.n      = 1;
    minex_func.f      = tmcmc_objlogp_gsl;
    minex_func.params = &fp;

    // SELECT ONE MINIMIZER STRATEGY
    /*	T = gsl_multimin_fminimizer_nmsimplex;*/
    T = gsl_multimin_fminimizer_nmsimplex2;
    /*	T = gsl_multimin_fminimizer_nmsimplex2rand;*/
    s = gsl_multimin_fminimizer_alloc (T, 1);
    gsl_multimin_fminimizer_set (s, &minex_func, x, ss);

    *fmin = 0;
    *xmin = 0.0;

    do {
        iter++;
        status = gsl_multimin_fminimizer_iterate(s);
        size   = gsl_multimin_fminimizer_size (s);
        status = gsl_multimin_test_size (size, options.Tol);
    } while (status == GSL_CONTINUE && iter < options.MaxIter);

    if (s->fval <= options.Tol) {
        *fmin = s->fval;
        *xmin = gsl_vector_get(s->x, 0);
    }

    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free (s);
}
