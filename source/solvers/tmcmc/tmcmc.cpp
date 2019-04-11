#include "korali.h"
#include <numeric>
#include <limits>
#include <chrono>

#include <gsl/gsl_sort_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_multimin.h>

/************************************************************************/
/*                  Constructor / Destructor Methods                    */
/************************************************************************/

Korali::Solver::TMCMC::TMCMC(nlohmann::json& js) : Korali::Solver::Base::Base(js)
{
 setConfiguration(js);

 // Initializing Data Variables
 double *LCmem  = (double*) calloc (_s*_k->N*_k->N, sizeof(double));
 local_cov = (double**) calloc ( _s, sizeof(double*));
 for (size_t pos = 0; pos < _s; ++pos)
 {
  local_cov[pos] = LCmem + pos*_k->N*_k->N;
  for (size_t i = 0; i < _k->N; i++) local_cov[pos][i*_k->N+i] = 1;
 }

 // Initializing Run Variables
 _varianceCoefficient        = 0;
 _annealingRatio              = 0;
 _uniqueSelections = 0;
 _uniqueEntries = _s;
 _logEvidence  = 0;
 _acceptanceRate     = 1.0;
 nChains = _s;
 finishedChains = 0;
 _varianceCoefficient = std::numeric_limits<double>::infinity();
 _covarianceMatrix =  (double*) calloc (_k->N*_k->N, sizeof(double));
 _meanTheta =  (double*) calloc (_k->N+1, sizeof(double));

 // Initializing TMCMC Leaders
 ccPoints    = (double*) calloc (_k->N*_s, sizeof(double));
 ccFitness   = (double*) calloc (_s, sizeof(double));

 clPoints    = (double*) calloc (_k->N*_s, sizeof(double));
 clFitness   = (double*) calloc (_s, sizeof(double));

 chainPendingFitness = (bool*) calloc (_s, sizeof(bool));
 chainCurrentStep    = (size_t*) calloc (_s, sizeof(size_t));
 chainLength         = (size_t*) calloc (_s, sizeof(size_t));

 databaseEntries  = 0;
 databasePoints   = (double*) calloc (_k->N*_s, sizeof(double));
 databaseFitness  = (double*) calloc (_s, sizeof(double));

 for (size_t c = 0; c < _s; c++) chainCurrentStep[c] = 0;
 for (size_t c = 0; c < _s; c++) chainLength[c] = 1 + _burnIn;
 for (size_t c = 0; c < _s; c++) chainPendingFitness[c] = false;

 // Setting Chain-Specific Seeds
 range = gsl_rng_alloc (gsl_rng_default);
 gsl_rng_set(range, _k->_seed++);

 chainGSLRange = (gsl_rng**) calloc (_s, sizeof(gsl_rng*));
 for (size_t c = 0; c < _s; c++)
 {
  chainGSLRange[c] = gsl_rng_alloc (gsl_rng_default);
  gsl_rng_set(chainGSLRange[c], _k->_seed++);
 }

  // TODO: Ensure proper memory deallocation
}

Korali::Solver::TMCMC::~TMCMC()
{

}

/************************************************************************/
/*                    Configuration Methods                             */
/************************************************************************/

nlohmann::json Korali::Solver::TMCMC::getConfiguration()
{
 auto js = this->Korali::Solver::Base::getConfiguration();

 js["Engine"] = "TMCMC";
 js["Population Size"] = _s;
 js["Current Generation"] = _currentGeneration;
 js["Covariance Tolerance"] = _tolCOV;
 js["Min Rho Update"] = _minStep;
 js["Covariance Scaling"] = _bbeta;
 js["Use Local Covariance"] = _useLocalCov;
 js["Burn In"] = _burnIn;
 js["Termination Criteria"]["Max Generations"] = _maxGens;;

 return js;
}

void Korali::Solver::TMCMC::setConfiguration(nlohmann::json& js)
{
 _currentGeneration = consume(js, { "Current Generation" }, KORALI_NUMBER, std::to_string(0));
 _s                 = consume(js, { "Population Size" }, KORALI_NUMBER);
 _tolCOV            = consume(js, { "Covariance Tolerance" }, KORALI_NUMBER, std::to_string(1.0));
 _minStep           = consume(js, { "Min Rho Update" }, KORALI_NUMBER, std::to_string(1e-9));
 _bbeta             = consume(js, { "Covariance Scaling" }, KORALI_NUMBER, std::to_string(0.005));
 _useLocalCov       = consume(js, { "Use Local Covariance" }, KORALI_BOOLEAN, "false");
 _burnIn            = consume(js, { "Burn In" }, KORALI_NUMBER, std::to_string(0));
 _maxGens           = consume(js, { "Termination Criteria", "Max Generations" }, KORALI_NUMBER, std::to_string(20));
}

/************************************************************************/
/*                   Load/Save State Methods                            */
/************************************************************************/

nlohmann::json Korali::Solver::TMCMC::getState()
{
  auto js = this->Korali::Solver::Base::getState();

  return js;
}

void Korali::Solver::TMCMC::setState(nlohmann::json js)
{
  this->Korali::Solver::Base::setState(js);
}

/************************************************************************/
/*                    Functional Methods                                */
/************************************************************************/

void Korali::Solver::TMCMC::run()
{
 double samplingTime = 0.0;
 double engineTime = 0.0;

 startTime = std::chrono::system_clock::now();

 // Setting initial conditions
 for (size_t c = 0; c < _s; c++)
  for (size_t d = 0; d < _k->N; d++)
   clPoints[c*_k->N + d] = ccPoints[c*_k->N + d] = _k->_parameters[d]->getRandomNumber();

 for(; _annealingRatio < 1.0 && _currentGeneration < _maxGens; _currentGeneration++)
 {
  auto t0 = std::chrono::system_clock::now();

  while (finishedChains < nChains)
  {
   for (size_t c = 0; c < nChains; c++) if (chainCurrentStep[c] < chainLength[c]) if (chainPendingFitness[c] == false)
   {
    chainPendingFitness[c] = true;
    generateCandidate(c);
    _k->_conduit->evaluateSample(ccPoints, c);
   }
   _k->_conduit->checkProgress();
  }

  t1 = std::chrono::system_clock::now();
  samplingTime += std::chrono::duration<double>(t1-t0).count();

  if (_k->_verbosity >= KORALI_MINIMAL) printf("[Korali] Generation %ld - Annealing Ratio:  %.2f%% - Elapsed Time: %fs\n", _currentGeneration, _annealingRatio*100, std::chrono::duration<double>(t1-startTime).count());

  t2 = std::chrono::system_clock::now();
  resampleGeneration();
  t3 = std::chrono::system_clock::now();
  engineTime += std::chrono::duration<double>(t3-t2).count();
 }

 auto endTime = std::chrono::system_clock::now();

 if (_currentGeneration == _maxGens) printf("[Korali] Finished (Max Generations Reached).\n");
 printf("[Korali] Total Time: %fs - Sampling Time: %fs - Engine Time: %fs.\n", std::chrono::duration<double>(endTime-startTime).count(), samplingTime, engineTime);

 saveResults();
}

void Korali::Solver::TMCMC::processSample(size_t c, double fitness)
{
 ccFitness[c] = fitness;
 double L = exp((ccFitness[c]-clFitness[c])*_annealingRatio);
 double P = gsl_ran_flat(chainGSLRange[c], 0.0, 1.0);

 if (P < L) {
   for (size_t i = 0; i < _k->N; i++) clPoints[c*_k->N + i] = ccPoints[c*_k->N + i];
   clFitness[c]  = ccFitness[c];
   _uniqueEntries++;
 }

 chainCurrentStep[c]++;
 if (chainCurrentStep[c] > _burnIn) updateDatabase(&clPoints[c*_k->N], clFitness[c]);
 chainPendingFitness[c] = false;
 if (chainCurrentStep[c] == chainLength[c]) finishedChains++;
}

void Korali::Solver::TMCMC::updateDatabase(double* point, double fitness)
{
 for (size_t i = 0; i < _k->N; i++) databasePoints[databaseEntries*_k->N + i] = point[i];    // Re-add burn-in
 databaseFitness[databaseEntries] = fitness;
 databaseEntries++;
}

void Korali::Solver::TMCMC::generateCandidate(int c)
{
 if (_currentGeneration == 0) return;

 double* covariance = _useLocalCov ? local_cov[c] : _covarianceMatrix;
 gsl_vector_view out_view = gsl_vector_view_array(&ccPoints[c*_k->N], _k->N);
 gsl_matrix_view sigma_view  = gsl_matrix_view_array(covariance, _k->N,_k->N);
 gsl_vector_view mean_view  = gsl_vector_view_array(&clPoints[c*_k->N], _k->N);
 gsl_ran_multivariate_gaussian(chainGSLRange[c], &mean_view.vector, &sigma_view.matrix, &out_view.vector);
}

void Korali::Solver::TMCMC::saveResults()
{
 double checksum = 0.0;

 for (size_t pos = 0; pos < databaseEntries; pos++)
  {
   for (size_t i = 0; i < _k->N; i++) checksum += databasePoints[pos*_k->N + i];
   checksum += databaseFitness[pos];
  }

 std::string outputName = "tmcmc.txt";
 printf("[Korali] Saving results to file: %s.\n", outputName.c_str());
 FILE *fp = fopen(outputName.c_str(), "w");
 for (size_t pos = 0; pos < databaseEntries; pos++)
 {
  for (size_t i = 0; i < _k->N; i++) fprintf(fp, "%3.12lf, ", databasePoints[pos*_k->N + i]);
  fprintf(fp, "%3.12lf\n", databaseFitness[pos]);
 }

 fclose(fp);
}

void Korali::Solver::TMCMC::resampleGeneration()
{
 double* flcp  = (double*) calloc (databaseEntries, sizeof(double));
 double* weight     = (double*) calloc (databaseEntries, sizeof(double));
 double* q = (double*) calloc (databaseEntries, sizeof(double));
 unsigned int* nn = (unsigned int*) calloc (databaseEntries, sizeof(unsigned int));
 size_t* sel = (size_t*) calloc (databaseEntries, sizeof(size_t));

 double fmin = 0, xmin = 0;
 minSearch(databaseFitness, databaseEntries, _annealingRatio, _tolCOV, &xmin, &fmin);

 double _prevAnnealingRatio = _annealingRatio;

 if (xmin > _prevAnnealingRatio)
 {
  _annealingRatio       = xmin;
  _varianceCoefficient = sqrt(fmin) + _tolCOV;
 }
 else
 {
  _annealingRatio       = _prevAnnealingRatio + _minStep;
  _varianceCoefficient = sqrt(tmcmc_objlogp(_annealingRatio, databaseFitness, databaseEntries, _prevAnnealingRatio, _tolCOV)) + _tolCOV;
 }

 if (_annealingRatio >= 1.0) { printf("[Korali] Finished (Annealing Ratio = 100%%).\n"); return; }

 /* Compute weights and normalize*/

 for (size_t i = 0; i < databaseEntries; i++) flcp[i] = databaseFitness[i]*(_annealingRatio-_prevAnnealingRatio);
 const double fjmax = gsl_stats_max(flcp, 1, databaseEntries);
 for (size_t i = 0; i < databaseEntries; i++) weight[i] = exp( flcp[i] - fjmax );

 double sum_weight = std::accumulate(weight, weight+databaseEntries, 0.0);
 _logEvidence  += log(sum_weight) + fjmax - log(databaseEntries);

 for (size_t i = 0; i < databaseEntries; i++) q[i] = weight[i]/sum_weight;

 for (size_t i = 0; i < databaseEntries; i++) sel[i] = 0;
 gsl_ran_multinomial(range, databaseEntries, _s, q, nn);
 for (size_t i = 0; i < databaseEntries; i++) sel[i] += nn[i];

 int zeroCount = 0;
 for (size_t i = 0; i < databaseEntries; i++) if (sel[i] == 0) zeroCount++;
 _uniqueSelections = databaseEntries - zeroCount;
 _acceptanceRate     = (1.0*_uniqueSelections)/_s;

 for (size_t i = 0; i < _k->N; i++)
 {
  _meanTheta[i] = 0;
  for (size_t j = 0; j < databaseEntries; j++) _meanTheta[i] += databasePoints[j*_k->N + i]*q[j];
 }

 double meanv[_k->N];
 for (size_t i = 0; i < _k->N; i++)  meanv[i] = _meanTheta[i];
 for (size_t i = 0; i < _k->N; i++) for (size_t j = i; j < _k->N; ++j)
 {
  double s = 0.0;
  for (size_t k = 0; k < databaseEntries; ++k) s += q[k]*(databasePoints[k*_k->N+i]-meanv[i])*(databasePoints[k*_k->N+j]-meanv[j]);
  _covarianceMatrix[i*_k->N + j] = _covarianceMatrix[j*_k->N + i] = s*_bbeta;
 }

 gsl_matrix_view sigma  = gsl_matrix_view_array(_covarianceMatrix, _k->N,_k->N);
 gsl_linalg_cholesky_decomp( &sigma.matrix );

 int newchains = 0;
 for (size_t i = 0; i < databaseEntries; i++) if (sel[i] != 0) newchains++;

 int ldi = 0;
 for (size_t i = 0; i < databaseEntries; i++) {
   if (sel[i] != 0) {
     for (size_t j = 0; j < _k->N ; j++) clPoints[ldi*_k->N + j] = databasePoints[i*_k->N + j];
     clFitness[ldi] = databaseFitness[i];
     chainLength[ldi] = sel[i] + _burnIn;
     ldi++;
   }
 }

 if (_useLocalCov) computeChainCovariances(local_cov, newchains);

 databaseEntries = 0;
 _uniqueEntries = 0;
 nChains = newchains;
 finishedChains = 0;
 for (size_t c = 0; c < nChains; c++) chainCurrentStep[c] = 0;
 for (size_t c = 0; c < nChains; c++) chainPendingFitness[c] = false;

 free(flcp);
 free(weight);
 free(q);
 free(nn);
 free(sel);
}

void Korali::Solver::TMCMC::computeChainCovariances(double** chain_cov, int newchains)
{
 printf("Precomputing chain covariances for the current generation...\n");

 // allocate space
 int* nn_ind  = (int*) calloc (newchains, sizeof(int));
 size_t* nn_count   = (size_t*) calloc (newchains, sizeof(size_t));
 double* diam    = (double*) calloc (_k->N, sizeof(double));
 double* chain_mean = (double*) calloc (_k->N, sizeof(double));
 gsl_matrix* work   = gsl_matrix_alloc(_k->N, _k->N);

 // find diameters
 for (size_t d = 0; d < _k->N; ++d) {
  double d_min = +1e6;
  double d_max = -1e6;
  for (size_t pos = 0; pos < _s; ++pos) {
   double s = databasePoints[pos*_k->N+d];
   if (d_min > s) d_min = s;
   if (d_max < s) d_max = s;
  }
  diam[d] = d_max-d_min;
  printf("Diameter %ld: %.6lf\n", d, diam[d]);
 }

 int idx, pos;
 int status = 0;
 double ds = 0.05;
 for (double scale = 0.1; scale <= 1.0; scale += ds) {
  // find neighbors in a rectangle - O(_s^2)
  for (pos = 0; pos < newchains; ++pos) {
   nn_count[pos] = 0;
   double* curr = &clPoints[pos*_k->N];
   for (size_t i = 0; i < _s; i++) {
    double* s = &databasePoints[i*_k->N];
    bool isInRectangle = true;
     for (size_t d = 0; d < _k->N; d++)  if (fabs(curr[d]-s[d]) > scale*diam[d]) isInRectangle = false;
     if (isInRectangle) {
      nn_ind[pos*_s+nn_count[pos]] = i;
      nn_count[pos]++;
     }
   }
  }

  // compute the covariances
  for (pos = 0; pos < newchains; ++pos) {
   for (size_t d = 0; d < _k->N; ++d) {
    chain_mean[d] = 0;
    for (size_t k = 0; k < nn_count[pos]; ++k) {
     idx = nn_ind[pos*_s+k];
     chain_mean[d] += databasePoints[idx*_k->N+d];
    }
    chain_mean[d] /= nn_count[pos];
   }

   for (size_t i = 0; i < _k->N; i++)
    for (size_t j = 0; j < _k->N; ++j) {
     double s = 0;
     for (size_t k = 0; k < nn_count[pos]; k++) {
      idx = nn_ind[pos*_s+k];
      s  += (databasePoints[idx*_k->N+i]-chain_mean[i]) *
         (databasePoints[idx*_k->N+j]-chain_mean[j]);
     }
     chain_cov[pos][i*_k->N+j] = chain_cov[pos][j*_k->N+i] = s/nn_count[pos];
    }

   // check if the matrix is positive definite
   for (size_t i = 0; i < _k->N; i++)
    for (size_t j = 0; j < _k->N; ++j) {
     double s = chain_cov[pos][i*_k->N+j];
     gsl_matrix_set(work, i, j, s);
    }
   gsl_set_error_handler_off();
   status = gsl_linalg_cholesky_decomp(work);
   if (status == GSL_SUCCESS) break;
  }
 }

 for (pos = 0; pos < newchains; ++pos) {
   gsl_matrix_view sigma  = gsl_matrix_view_array(chain_cov[pos], _k->N,_k->N);
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

double Korali::Solver::TMCMC::tmcmc_objlogp(double x, const double *fj, int fn, double pj, double zero)
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

double Korali::Solver::TMCMC::objLog(const gsl_vector *v, void *param)
{
 double x = gsl_vector_get(v, 0);
 fparam_t *fp = (fparam_t *) param;
 return Korali::Solver::TMCMC::tmcmc_objlogp(x, fp->fj, fp->fn, fp->pj, fp->tol);
}

void Korali::Solver::TMCMC::minSearch(double const *fj, int fn, double pj, double objTol, double *xmin, double *fmin)
{
 // Minimizer Options
 size_t MaxIter     = 100;    /* Max number of search iterations */
 double Tol         = 1e-12;  /* Tolerance for root finding */
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
