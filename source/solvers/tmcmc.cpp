#include "solvers/tmcmc.h"
#include <numeric>
#include <limits>
#include <chrono>

#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multimin.h>

Korali::TMCMC::TMCMC(BaseProblem* problem) : Korali::Conduit::Conduit(problem)
{
 TolCOV  = 1;
 MinStep = 1e-9;
 bbeta   = 0.005;
 _useLocalCov = false;
 _verbose = false;
}

void Korali::TMCMC::runSolver()
{
 printf("[Korali] TMCMC - Parameters: %ld, Seed: %ld\n", N, _problem->_seed);
 double samplingTime = 0.0;
 double engineTime = 0.0;
 initializeEngine();

 auto startTime = std::chrono::system_clock::now();

 for(_currentGeneration = 0; _annealingRatio < 1.0 && _currentGeneration < _maxGens; _currentGeneration++)
 {
  auto t0 = std::chrono::system_clock::now();

  while (finishedChains < nChains)
  {
   for (int c = 0; c < nChains; c++) if (chainCurrentStep[c] < chainLength[c]) if (chainPendingFitness[c] == false)
   {
    chainPendingFitness[c] = true;
    if(generateCandidate(c)) evaluateSample(c);  else processSample(c, -DBL_MAX);
   }
   checkProgress();
  }

  auto t1 = std::chrono::system_clock::now();
  samplingTime += std::chrono::duration<double>(t1-t0).count();

  printf("[Korali] Generation %d - Time: %fs, Annealing: %.2f%%, Acceptance: %.2f%%\n", _currentGeneration, std::chrono::duration<double>(t1-t0).count(), _annealingRatio*100, _acceptanceRate*100);

  auto t2 = std::chrono::system_clock::now();
  resampleGeneration();
  auto t3 = std::chrono::system_clock::now();
  engineTime += std::chrono::duration<double>(t3-t2).count();
 }

 auto endTime = std::chrono::system_clock::now();

 if (_currentGeneration == _maxGens) printf("[Korali] Finished (Max Generations Reached).\n");
 printf("[Korali] Total Time: %fs - Sampling Time: %fs - Engine Time: %fs.\n", std::chrono::duration<double>(endTime-startTime).count(), samplingTime, engineTime);

 saveResults();
}

void Korali::TMCMC::processSample(size_t c, double fitness)
{
 ccFitness[c] = fitness;
 ccLogPrior[c] = _problem->getPriorsLogProbabilityDensity(&ccPoints[c*N]);
 double L = exp((ccLogPrior[c]-clLogPrior[c])+(ccFitness[c]-clFitness[c])*_annealingRatio);
 double P = gsl_ran_flat(chainGSLRange[c], 0.0, 1.0);

 if (P < L) {
   for (int i = 0; i < N; i++) clPoints[c*N + i] = ccPoints[c*N + i];
   clFitness[c]  = ccFitness[c];
   clLogPrior[c] = ccLogPrior[c];
   _uniqueEntries++;
 }

 updateDatabase(&clPoints[c*N], clFitness[c]);
 chainCurrentStep[c]++;
 chainPendingFitness[c] = false;
 if (chainCurrentStep[c] == chainLength[c]) finishedChains++;
}

void Korali::TMCMC::updateDatabase(double* point, double fitness)
{
 for (int i = 0; i < N; i++) databasePoints[databaseEntries*N + i] = point[i];    // Re-add burn-in
 databaseFitness[databaseEntries] = fitness;
 databaseEntries++;
}

bool Korali::TMCMC::generateCandidate(int c)
{
 if (_currentGeneration == 0) return true;

 double* covariance = _useLocalCov ? local_cov[c] : _covarianceMatrix;
 gsl_vector_view out_view = gsl_vector_view_array(&ccPoints[c*N], N);
 gsl_matrix_view sigma_view  = gsl_matrix_view_array(covariance, N,N);
 gsl_vector_view mean_view  = gsl_vector_view_array(&clPoints[c*N], N);
 gsl_ran_multivariate_gaussian(chainGSLRange[c], &mean_view.vector, &sigma_view.matrix, &out_view.vector);

 for (int i = 0; i < N; i++) if (ccPoints[c*N + i] < _problem->_parameters[i]->_lowerBound || ccPoints[c*N + i] > _problem->_parameters[i]->_upperBound) return false;

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

void Korali::TMCMC::initializeEngine()
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
 _varianceCoefficient        = 0;
 _annealingRatio              = 0;
 _uniqueSelections = 0;
 _uniqueEntries = _sampleCount;
 _logSelections  = 0;
 _acceptanceRate     = 1.0;
 _currentGeneration = 0;
 _varianceCoefficient = std::numeric_limits<double>::infinity();
 _covarianceMatrix =  (double*) calloc (N*N, sizeof(double));
 _meanTheta =  (double*) calloc (N+1, sizeof(double));

 // Initializing TMCMC Leaders
 ccPoints    = getSampleArrayPointer();
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
 for (int c = 0; c < _sampleCount; c++) for (int d = 0; d < N; d++)  clPoints[c*N + d] = ccPoints[c*N + d] = _problem->_parameters[d]->getRandomNumber();
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

void Korali::TMCMC::resampleGeneration()
{
 double* flcp  = (double*) calloc (databaseEntries, sizeof(double));
 double* weight     = (double*) calloc (databaseEntries, sizeof(double));
 double* q = (double*) calloc (databaseEntries, sizeof(double));
 unsigned int* nn = (unsigned int*) calloc (databaseEntries, sizeof(unsigned int));
 size_t* sel = (size_t*) calloc (databaseEntries, sizeof(size_t));

 double fmin = 0, xmin = 0;
 minSearch(databaseFitness, databaseEntries, _annealingRatio, TolCOV, &xmin, &fmin);

 double _prevAnnealingRatio = _annealingRatio;

 if (xmin > _prevAnnealingRatio)
 {
  _annealingRatio       = xmin;
  _varianceCoefficient = sqrt(fmin) + TolCOV;
 }
 else
 {
  _annealingRatio       = _prevAnnealingRatio + MinStep;
  _varianceCoefficient = sqrt(tmcmc_objlogp(_annealingRatio, databaseFitness, databaseEntries, _prevAnnealingRatio, TolCOV)) + TolCOV;
 }

 if (_annealingRatio >= 1.0) { printf("[Korali] Finished (Annealing Ratio = 100%).\n"); return; }

 /* Compute weights and normalize*/

 for (int i = 0; i < databaseEntries; i++) flcp[i] = databaseFitness[i]*(_annealingRatio-_prevAnnealingRatio);
 const double fjmax = gsl_stats_max(flcp, 1, databaseEntries);
 for (int i = 0; i < databaseEntries; i++) weight[i] = exp( flcp[i] - fjmax );

 double sum_weight = std::accumulate(weight, weight+databaseEntries, 0.0);
 _logSelections  = log(sum_weight) + fjmax - log(databaseEntries);

 for (int i = 0; i < databaseEntries; i++) q[i] = weight[i]/sum_weight;

 for (int i = 0; i < databaseEntries; i++) sel[i] = 0;
 gsl_ran_multinomial(range, databaseEntries, _sampleCount, q, nn);
 for (int i = 0; i < databaseEntries; i++) sel[i] += nn[i];

 int zeroCount = 0;
 for (int i = 0; i < databaseEntries; i++) if (sel[i] == 0) zeroCount++;
 _uniqueSelections = databaseEntries - zeroCount;
 _acceptanceRate     = (1.0*_uniqueSelections)/_sampleCount;

 for (int i = 0; i < N; i++)
 {
  _meanTheta[i] = 0;
  for (int j = 0; j < databaseEntries; j++) _meanTheta[i] += databasePoints[j*N + i]*q[j];
 }

 double meanv[N];
 for (int i = 0; i < N; i++)  meanv[i] = _meanTheta[i];
 for (int i = 0; i < N; i++) for (int j = i; j < N; ++j)
 {
  double s = 0.0;
  for (size_t k = 0; k < databaseEntries; ++k) s += q[k]*(databasePoints[k*N+i]-meanv[i])*(databasePoints[k*N+j]-meanv[j]);
  _covarianceMatrix[i*N + j] = _covarianceMatrix[j*N + i] = s*bbeta;
 }

 gsl_matrix_view sigma  = gsl_matrix_view_array(_covarianceMatrix, N,N);
 gsl_linalg_cholesky_decomp( &sigma.matrix );

 int newchains = 0;
 for (int i = 0; i < databaseEntries; i++) if (sel[i] != 0) newchains++;

 int ldi = 0;
 for (int i = 0; i < databaseEntries; i++) {
   if (sel[i] != 0) {
     for (int _annealingRatio = 0; _annealingRatio < N ; _annealingRatio++) clPoints[ldi*N + _annealingRatio] = databasePoints[i*N + _annealingRatio];
     clFitness[ldi] = databaseFitness[i];
     chainLength[ldi] = sel[i];
     ldi++;
   }
 }

 if (_useLocalCov) computeChainCovariances(local_cov, newchains);

 databaseEntries = 0;
 _uniqueEntries = 0;
 nChains = newchains;
 finishedChains = 0;
 for (int c = 0; c < nChains; c++) chainCurrentStep[c] = 0;
 for (int c = 0; c < nChains; c++) chainPendingFitness[c] = false;

 free(flcp);
 free(weight);
 free(q);
 free(nn);
 free(sel);
}

void Korali::TMCMC::computeChainCovariances(double** chain_cov, int newchains)
{
 if (_verbose) printf("Precomputing chain covariances for the current generation...\n");

 // allocate space
 int* nn_ind  = (int*) calloc (newchains, sizeof(int));
 int* nn_count   = (int*) calloc (newchains, sizeof(int));
 double* diam    = (double*) calloc (N, sizeof(double));
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
  if (_verbose) printf("Diameter %d: %.6lf\n", d, diam[d]);
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
   gsl_matrix_view sigma  = gsl_matrix_view_array(chain_cov[pos], N,N);
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

double Korali::TMCMC::objLog(const gsl_vector *v, void *param)
{
 double x = gsl_vector_get(v, 0);
 fparam_t *fp = (fparam_t *) param;
 return Korali::TMCMC::tmcmc_objlogp(x, fp->fj, fp->fn, fp->pj, fp->tol);
}

void Korali::TMCMC::minSearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin)
{
 // Minimizer Options
 size_t MaxIter     = 100;    /* Max number of search iterations */
 double Tol         = 1e-12;  /* Tolerance for root finding */
 bool   Display     = false;  /* Print output */
 double Step        = 1e-8;   /* Search stepsize */

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
 gsl_vector_set_all (ss, Step);

 minex_func.n      = 1;
 minex_func.f      = objLog;
 minex_func.params = &fp;

 // SELECT ONE MINIMIZER STRATEGY
 /* T = gsl_multimin_fminimizer_nmsimplex;*/
 T = gsl_multimin_fminimizer_nmsimplex2;
 /* T = gsl_multimin_fminimizer_nmsimplex2rand;*/
 s = gsl_multimin_fminimizer_alloc (T, 1);
 gsl_multimin_fminimizer_set (s, &minex_func, x, ss);

 *fmin = 0;
 *xmin = 0.0;

 do {
   iter++;
   status = gsl_multimin_fminimizer_iterate(s);
   size   = gsl_multimin_fminimizer_size (s);
   status = gsl_multimin_test_size (size, Tol);
 } while (status == GSL_CONTINUE && iter < MaxIter);

 if (s->fval <= Tol) {
   *fmin = s->fval;
   *xmin = gsl_vector_get(s->x, 0);
 }

 gsl_vector_free(x);
 gsl_vector_free(ss);
 gsl_multimin_fminimizer_free (s);
}
