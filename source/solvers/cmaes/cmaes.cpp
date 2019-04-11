#include "korali.h"
#include <chrono>

void Korali::Solver::CMAES::initialize()
{
 this->Korali::Solver::Base::initialize();

 // Initializing Gaussian Generator
 _gaussianGenerator = new Parameter::Gaussian();
 _gaussianGenerator->setDistribution(0.0, 1.0);
 _gaussianGenerator->initialize(_k->_seed++);

 // Getting sample vector pointer
 _samplePopulation =  (double*) calloc (sizeof(double), _k->N*_s);

 // Initializing MU and its weights

 if (_mu == 0) _mu = _s;
   _muWeights = new double[_mu];

 if (_muType == "LinearDecreasing") for (size_t i = 0; i < _mu; i++)  _muWeights[i] = _mu - i;
 if (_muType == "Equal")            for (size_t i = 0; i < _mu; i++)  _muWeights[i] = 1;
 if (_muType == "Logarithmic")      for (size_t i = 0; i < _mu; i++)  _muWeights[i] = log(_mu+1.)-log(i+1.);

 /* normalize weights vector and set mueff */
 double s1 = 0.0;
 double s2 = 0.0;

 for (size_t  i = 0; i < _mu; i++)
 {
  s1 += _muWeights[i];
  s2 += _muWeights[i]*_muWeights[i];
 }

 _muEffective = s1*s1/s2;

 for (size_t i = 0; i < _mu; i++) _muWeights[i] /= s1;

 if(_mu < 1 || _mu > _s || (_mu == _s && _muWeights[0] == _muWeights[_mu-1]))
 { fprintf( stderr, "[Korali] Error: Invalid setting of Mu (%lu) and/or Lambda (%lu)\n", _mu, _s); exit(-1); }

 // Setting MU Covariance
 if (_muCovariance < 1) _muCovariance = _muEffective;

 // Checking Covariance Matrix Evaluation Frequency

 if (_diagonalCovarianceMatrixEvalFrequency == 0)  _diagonalCovarianceMatrixEvalFrequency = 2 + 100. * _k->N / sqrt((double)_s);
 if (_diagonalCovarianceMatrixEvalFrequency < 1)
 { fprintf( stderr, "[Korali] Error: Matrix covariance evaluation frequency is less than 1 (%lu)\n", _diagonalCovarianceMatrixEvalFrequency); exit(-1); }

 // Setting Sigma Cumulation Factor

 double tmpSigma = _sigmaCumulationFactor;
 if (tmpSigma > 0) _sigmaCumulationFactor *= (_muEffective + 2.0) / (_k->N + _muEffective + 3.0);
 if (tmpSigma <= 0 || tmpSigma >= 1)  _sigmaCumulationFactor = (_muEffective + 2.) / (_k->N + _muEffective + 3.0);

 // Setting Damping Factor

 if (_dampFactor < 0) _dampFactor = 1;
 _dampFactor = _dampFactor* (1 + 2*std::max(0.0, sqrt((_muEffective-1.0)/(_k->N+1.0)) - 1))  /* basic factor */
  * std::max(0.3, 1. - (double)_k->N / (1e-6+std::min(_maxGens, _maxFitnessEvaluations/_s)))
  + _sigmaCumulationFactor;             /* minor increment */

 // Setting Cumulative Covariance

 if (_cumulativeCovariance <= 0 || _cumulativeCovariance> 1)  _cumulativeCovariance = 4. / (_k->N + 4);

 // Set covariance Matrix Learning Rate

 double t1 = 2. / ((_k->N+1.4142)*(_k->N+1.4142));
 double t2 = (2.*_muEffective-1.) / ((_k->N+2.)*(_k->N+2.)+_muEffective);
 t2 = (t2 > 1) ? 1 : t2;
 t2 = (1./_muCovariance) * t1 + (1.-1./_muCovariance) * t2;

 double tmpCovarianceRate = _covarianceMatrixLearningRate;
 if (tmpCovarianceRate >= 0)   _covarianceMatrixLearningRate *= t2;
 if (tmpCovarianceRate < 0 || tmpCovarianceRate > 1)  _covarianceMatrixLearningRate = t2;

 // Setting eigensystem evaluation Frequency
   _covarianceEigenEvalFreq = floor(1.0/(double)_covarianceMatrixLearningRate/((double)_k->N)/10.0);

 double trace = 0.0;
 for (size_t i = 0; i < _k->N; ++i)   trace += _k->_parameters[i]->_initialStdDev*_k->_parameters[i]->_initialStdDev;
 //if (!_silent) printf("Trace: %f\n", trace);
 sigma = sqrt(trace/_k->N); /* _muEffective/(0.2*_muEffective+sqrt(_k->N)) * sqrt(trace/_k->N); */

 flgEigensysIsUptodate = true;

  countevals = 0;
  currentBest = 0.0;

  rgpc = (double*) calloc (sizeof(double), _k->N);
  rgps = (double*) calloc (sizeof(double), _k->N);
  rgdTmp = (double*) calloc (sizeof(double), _k->N);
  rgBDz = (double*) calloc (sizeof(double), _k->N);
  rgxmean = (double*) calloc (sizeof(double), _k->N);
  rgxold = (double*) calloc (sizeof(double), _k->N);
  rgxbestever = (double*) calloc (sizeof(double), _k->N);
  rgout = (double*) calloc (sizeof(double), _k->N);
  rgD = (double*) calloc (sizeof(double), _k->N);
  C = (double**) calloc (sizeof(double*), _k->N);
  B = (double**)calloc (sizeof(double*), _k->N);
  rgFuncValue = (double*) calloc (sizeof(double), _s);

  for (size_t i = 0; i < _k->N; ++i) {
   C[i] = (double*) calloc (sizeof(double), _k->N);
   B[i] = (double*) calloc (sizeof(double), _k->N);
  }
  index = (int *) calloc (sizeof(int*), _s);
  for (size_t i = 0; i < _s; ++i)  index[i] = i; /* should not be necessary */
  curBest = (double *) calloc (sizeof(double), _s);

 for (size_t i = 0; i < _k->N; ++i)
 {
  B[i][i] = 1.;
  C[i][i] = rgD[i] = _k->_parameters[i]->_initialStdDev * sqrt(_k->N / trace);
  C[i][i] *= C[i][i];
 }

 minEW = doubleRangeMin(rgD, _k->N); minEW = minEW * minEW;
 maxEW = doubleRangeMax(rgD, _k->N); maxEW = maxEW * maxEW;

 maxdiagC=C[0][0]; for(size_t i=1;i<_k->N;++i) if(maxdiagC<C[i][i]) maxdiagC=C[i][i];
 mindiagC=C[0][0]; for(size_t i=1;i<_k->N;++i) if(mindiagC>C[i][i]) mindiagC=C[i][i];

 /* set rgxmean */
 for (size_t i = 0; i < _k->N; ++i)
 {
   if(_k->_parameters[i]->_initialValue < _k->_parameters[i]->_lowerBound || _k->_parameters[i]->_initialValue > _k->_parameters[i]->_upperBound)
    fprintf(stderr,"[Korali] Warning: Initial Value (%.4f) for \'%s\' is out of bounds (%.4f-%.4f).\n", _k->_parameters[i]->_initialValue, _k->_parameters[i]->_name.c_str(), _k->_parameters[i]->_lowerBound, _k->_parameters[i]->_upperBound);
   rgxmean[i] = rgxold[i] = _k->_parameters[i]->_initialValue;
 }

 _initializedSample = (bool*) calloc (_s, sizeof(bool));
 _fitnessVector = (double*) calloc (sizeof(double), _s);
}

void Korali::Solver::CMAES::saveGeneration()
{
 FILE *fid = NULL;

 size_t filenum = 0;
 while(fid == NULL)
 {
  if (filenum > 100) { printf("[Korali] Error: Too many result files. Backup your previous results and run again.\n"); exit(-1);}
  sprintf(filepath, "korali%05lu.json",filenum);
  fid = fopen(filepath, "w");
  filenum++;
 }

 fprintf(fid, getState().dump(1).c_str());
 fclose(fid);
}

void Korali::Solver::CMAES::run()
{
 if (_k->_verbosity >= KORALI_MINIMAL) printf("[Korali] Starting CMA-ES.\n");

 startTime = std::chrono::system_clock::now();
 saveGeneration();

 while(!checkTermination())
 {
  prepareGeneration();

   while (_finishedSamples < _s)
   {
    for (size_t i = 0; i < _s; i++) if (_initializedSample[i] == false)
    {
     _initializedSample[i] = true;
     _k->_conduit->evaluateSample(_samplePopulation, i);
    }
    _k->_conduit->checkProgress();
   }
  updateDistribution(_fitnessVector);

  t1 = std::chrono::system_clock::now();
  if (_k->_verbosity >= KORALI_NORMAL) printf("[Korali] Generation %ld - Elapsed Time: %fs\n", _currentGeneration, std::chrono::duration<double>(t1-startTime).count());
  saveGeneration();
 }

 endTime = std::chrono::system_clock::now();

 if (_k->_verbosity >= KORALI_MINIMAL) printf("[Korali] Finished - Reason: %s\n", _terminationReason);
 if (_k->_verbosity >= KORALI_MINIMAL)  for (size_t i = 0; i < _k->N; i++)  printf("[Korali] Best Value For \'%s\' = %g\n", _k->_parameters[i]->_name.c_str(), rgxbestever[i]);
 if (_k->_verbosity >= KORALI_MINIMAL) printf("[Korali] Total Elapsed Time: %fs\n", std::chrono::duration<double>(endTime-startTime).count());
 if (_k->_verbosity >= KORALI_MINIMAL) printf("[Korali] Saving results to \'%s\'\n", filepath);
}

void Korali::Solver::CMAES::processSample(size_t sampleId, double fitness)
{
 _fitnessVector[sampleId] = -fitness;
 _finishedSamples++;
}

bool Korali::Solver::CMAES::isFeasible(double *pop)
{
 for (size_t i = 0; i < _k->N; i++)
  if (pop[i] < _k->_parameters[i]->_lowerBound || pop[i] > _k->_parameters[i]->_upperBound)  return false;
 return true;
}

void Korali::Solver::CMAES::prepareGeneration()
{
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= _currentGeneration));

 /* calculate eigensystem  */
 if (!flgEigensysIsUptodate) {
  if (!flgdiag)
   updateEigensystem(0);
  else {
   for (size_t i = 0; i < _k->N; ++i)
    rgD[i] = sqrt(C[i][i]);
   minEW = doubleRangeMin(rgD, _k->N) * doubleRangeMin(rgD, _k->N);
   maxEW = doubleRangeMax(rgD, _k->N) * doubleRangeMin(rgD, _k->N);
   flgEigensysIsUptodate = true;
  }
 }

 /* treat minimal standard deviations and numeric problems */
 for (size_t i = 0; i < _k->N; ++i)
  while (sigma * sqrt(C[i][i]) < _k->_parameters[i]->_minStdDevChange)
   sigma *= exp(0.05+_sigmaCumulationFactor/_dampFactor);

 for (size_t iNk = 0; iNk < _s; ++iNk)
 { /* generate scaled random vector (D * z) */
  for (size_t i = 0; i < _k->N; ++i)
  {
   if (flgdiag) _samplePopulation[iNk * _k->N + i] = rgxmean[i] + sigma * rgD[i] * _gaussianGenerator->getRandomNumber();
   else rgdTmp[i] = rgD[i] * _gaussianGenerator->getRandomNumber();
  }

  if (!flgdiag)
   for (size_t i = 0; i < _k->N; ++i) {
   double sum = 0.0;
    for (size_t j = 0; j < _k->N; ++j)
     sum += B[i][j] * rgdTmp[j];
    _samplePopulation[iNk * _k->N + i] = rgxmean[i] + sigma * sum;
   }
 }

 _currentGeneration++;

 for(size_t i = 0; i < _s; ++i) while( !isFeasible(&_samplePopulation[i*_k->N] )) reSampleSingle(i);

 _finishedSamples = 0;
 for (size_t i = 0; i < _s; i++) _initializedSample[i] = false;
}


void Korali::Solver::CMAES::reSampleSingle(size_t idx)
{
 double *rgx;

 if (idx < 0 || idx >= _s)  fprintf(stderr, "[Korali] Error: reSampleSingle(): Population member \n");
 rgx = &_samplePopulation[idx*_k->N];

 for (size_t i = 0; i < _k->N; ++i)  rgdTmp[i] = rgD[i] * _gaussianGenerator->getRandomNumber();

 /* add mutation (sigma * B * (D*z)) */
 for (size_t i = 0; i < _k->N; ++i) {
 double sum = 0.0;
  for (size_t j = 0; j < _k->N; ++j)
   sum += B[i][j] * rgdTmp[j];
  rgx[i] = rgxmean[i] + sigma * sum;
 }
}


void Korali::Solver::CMAES::updateDistribution(const double *fitnessVector)
{
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= _currentGeneration));
 countevals += _s;

 /* assign function values */
 for (size_t i = 0; i < _s; i++) curBest[i] = rgFuncValue[i] = fitnessVector[i];

 /* Generate index */
 sorted_index(fitnessVector, index, _s);

 /* Test if function values are identical, escape flat fitness */
 if (rgFuncValue[index[0]] == rgFuncValue[index[(int)_s/2]]) {
  sigma *= exp(0.2+_sigmaCumulationFactor/_dampFactor);
  fprintf(stderr, "[Korali] Error: Warning: sigma increased due to equal function values.\n");
  fprintf(stderr, "[Korali] Reconsider the formulation of the objective function\n");
 }

 /* update function value history */
 prevFunctionValue = currentFunctionValue;
 currentFunctionValue = fitnessVector[index[0]];

 /* update xbestever */
 if (currentBest > curBest[index[0]] || _currentGeneration == 1)
 {
  for (size_t i = 0; i < _k->N; ++i) rgxbestever[i] = _samplePopulation[index[0]*_k->N + i];
  currentBest = curBest[index[0]];
 }

 /* calculate rgxmean and rgBDz~_k->N(0,C) */
 for (size_t i = 0; i < _k->N; ++i) {
  rgxold[i] = rgxmean[i];
  rgxmean[i] = 0.;
  for (size_t iNk = 0; iNk < _mu; ++iNk)
   rgxmean[i] += _muWeights[iNk] * _samplePopulation[index[iNk]*_k->N + i];
  rgBDz[i] = sqrt(_muEffective)*(rgxmean[i] - rgxold[i])/sigma;
 }

 double sum = 0.0;
 /* calculate z := D^(-1) * B^(-1) * rgBDz into rgdTmp */
 for (size_t i = 0; i < _k->N; ++i) {
  if (!flgdiag)
   for (size_t j = 0, sum = 0.; j < _k->N; ++j)
    sum += B[j][i] * rgBDz[j];
  else
   sum = rgBDz[i];
  rgdTmp[i] = sum / rgD[i];
 }

 /* cumulation for sigma (ps) using B*z */
 for (size_t i = 0; i < _k->N; ++i) {
  if (!flgdiag)
   for (size_t j = 0, sum = 0.; j < _k->N; ++j)
    sum += B[i][j] * rgdTmp[j];
  else
   sum = rgdTmp[i];
  rgps[i] = (1. - _sigmaCumulationFactor) * rgps[i] +
   sqrt(_sigmaCumulationFactor * (2. - _sigmaCumulationFactor)) * sum;
 }

 /* calculate norm(ps)^2 */
 double psxps = 0.0;
 for (size_t i = 0; i < _k->N; ++i)  psxps += rgps[i] * rgps[i];

 /* cumulation for covariance matrix (pc) using B*D*z~_k->N(0,C) */
 double chiN = sqrt((double) _k->N) * (1. - 1./(4.*_k->N) + 1./(21.*_k->N*_k->N));
 int hsig = sqrt(psxps) / sqrt(1. - pow(1.-_sigmaCumulationFactor, 2*_currentGeneration)) / chiN  < 1.4 + 2./(_k->N+1);

 for (size_t i = 0; i < _k->N; ++i)
  rgpc[i] = (1. - _cumulativeCovariance) * rgpc[i] +  hsig * sqrt(_cumulativeCovariance * (2. - _cumulativeCovariance)) * rgBDz[i];

 /* update of C  */
 adaptC2(hsig);

 /* update of sigma */
 sigma *= exp(((sqrt(psxps)/chiN)-1.)*_sigmaCumulationFactor/_dampFactor);
}

void Korali::Solver::CMAES::adaptC2(int hsig)
{
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= _currentGeneration));

 if (_covarianceMatrixLearningRate != 0.0)
 {
  /* definitions for speeding up inner-most loop */
  double ccov1 = std::min(_covarianceMatrixLearningRate * (1./_muCovariance) * (flgdiag ? (_k->N+1.5) / 3. : 1.), 1.);
  double ccovmu = std::min(_covarianceMatrixLearningRate * (1-1./_muCovariance)* (flgdiag ? (_k->N+1.5) / 3. : 1.), 1.-ccov1);
  double sigmasquare = sigma * sigma;

  flgEigensysIsUptodate = false;

  /* update covariance matrix */
  for (size_t i = 0; i < _k->N; ++i)
   for (size_t j = flgdiag ? i : 0; j <= i; ++j) {
   C[i][j] = (1 - ccov1 - ccovmu) * C[i][j] + ccov1 * (rgpc[i] * rgpc[j] + (1-hsig)*_cumulativeCovariance*(2.-_cumulativeCovariance) * C[i][j]);
   for (size_t k = 0; k < _mu; ++k) C[i][j] += ccovmu * _muWeights[k] * (_samplePopulation[index[k]*_k->N + i] - rgxold[i]) * (_samplePopulation[index[k]*_k->N + j] - rgxold[j]) / sigmasquare;
   }

  /* update maximal and minimal diagonal value */
  maxdiagC = mindiagC = C[0][0];
  for (size_t i = 1; i < _k->N; ++i) {
  if (maxdiagC < C[i][i]) maxdiagC = C[i][i];
  else if (mindiagC > C[i][i])  mindiagC = C[i][i];
  }
 } /* if ccov... */
}

bool Korali::Solver::CMAES::checkTermination()
{
 double fac;
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= _currentGeneration));
 bool terminate = false;

 /* function value reached */
 if (_currentGeneration > 1  &&   rgFuncValue[index[0]] <= _stopMinFitness)
 {
  terminate = true;
  sprintf(_terminationReason, "Fitness Value (%7.2e) < (%7.2e)",  rgFuncValue[index[0]], _stopMinFitness);
 }

 /* TolFun */
  double range = fabs(currentFunctionValue - prevFunctionValue);
  if (_currentGeneration > 0 && range <= _stopFitnessDiffThreshold) {
  terminate = true;
  sprintf(_terminationReason, "Function value differences (%7.2e) < (%7.2e)",  range, _stopFitnessDiffThreshold);
 }

 size_t cTemp = 0;
 for(size_t i=0; i<_k->N; ++i) {
  cTemp += (sigma * sqrt(C[i][i]) < _stopMinDeltaX) ? 1 : 0;
  cTemp += (sigma * rgpc[i] < _stopMinDeltaX) ? 1 : 0;
 }

 if (cTemp == 2*_k->N) {
  terminate = true;
  sprintf(_terminationReason, "Object variable changes < %7.2e", _stopMinDeltaX);
 }

  size_t iAchse = 0;
  size_t iKoo = 0;
  if (!flgdiag) {
  for (iAchse = 0; iAchse < _k->N; ++iAchse)
  {
   fac = 0.1 * sigma * rgD[iAchse];
   for (iKoo = 0; iKoo < _k->N; ++iKoo){
    if (rgxmean[iKoo] != rgxmean[iKoo] + fac * B[iKoo][iAchse])
     break;
   }
   if (iKoo == _k->N)
   {
    terminate = true;
    sprintf(_terminationReason, "Standard deviation 0.1*%7.2e in principal axis %ld without effect.", fac/0.1, iAchse);
    break;
   } /* if (iKoo == _k->N) */
  } /* for iAchse    */
 } /* if flgdiag */

 /* Component of rgxmean is not changed anymore */
 for (iKoo = 0; iKoo < _k->N; ++iKoo)
 {
  if (rgxmean[iKoo] == rgxmean[iKoo] +
    0.2*sigma*sqrt(C[iKoo][iKoo]))
  {
   /* C[iKoo][iKoo] *= (1 + _covarianceMatrixLearningRate); */
   /* flg = 1; */
   terminate = true;
   sprintf(_terminationReason, "Standard deviation 0.2*%7.2e in coordinate %ld without effect.", sigma*sqrt(C[iKoo][iKoo]), iKoo);
   break;
  }

 } /* for iKoo */

 if(countevals >= _maxFitnessEvaluations)
 {
  terminate = true;
  sprintf(_terminationReason, "Conducted %lu function evaluations >= (%lu).", countevals, _maxFitnessEvaluations); }

 if(_currentGeneration >= _maxGens)
 {
  terminate = true;
  sprintf(_terminationReason, "Maximum number of Generations reached (%lu).", _maxGens);
 }

 return terminate;
}

void Korali::Solver::CMAES::updateEigensystem(int flgforce)
{
 if(flgforce == 0 && flgEigensysIsUptodate) return;

 eigen(_k->N, C, rgD, B);

 /* find largest and smallest eigenvalue, they are supposed to be sorted anyway */
 minEW = doubleRangeMin(rgD, _k->N);
 maxEW = doubleRangeMax(rgD, _k->N);

 for (size_t i = 0; i < _k->N; ++i)  rgD[i] = sqrt(rgD[i]);

 flgEigensysIsUptodate = true;
}

void Korali::Solver::CMAES::eigen(size_t size,  double **C, double *diag, double **Q)
{
 double* data = (double*) malloc (sizeof(double) * size * size);

 for (size_t i = 0; i <  size; i++)
 for (size_t j = 0; j <= i; j++)
 {
  data[i*size + j] = C[i][j];
  data[j*size + i] = C[i][j];
 }

 gsl_matrix_view m  = gsl_matrix_view_array (data, size, size);
 gsl_vector *eval = gsl_vector_alloc (size);
 gsl_matrix *evec = gsl_matrix_alloc (size, size);
 gsl_eigen_symmv_workspace * w =  gsl_eigen_symmv_alloc (size);
 gsl_eigen_symmv (&m.matrix, eval, evec, w);
 gsl_eigen_symmv_free (w);
 gsl_eigen_symmv_sort (eval, evec, GSL_EIGEN_SORT_ABS_ASC);

 for (size_t i = 0; i < size; i++)
 {
  gsl_vector_view evec_i = gsl_matrix_column (evec, i);
  for (size_t j = 0; j < size; j++) Q[j][i] =  -gsl_vector_get (&evec_i.vector, j);
 }

 for (size_t i = 0; i < size; i++) diag[i] = gsl_vector_get (eval, i);

 gsl_vector_free (eval);
 gsl_matrix_free (evec);
 free(data);
}

int Korali::Solver::CMAES::maxIdx(const double *rgd, int len)
{
 int i, res;
 for(i=1, res=0; i<len; ++i)
  if(rgd[i] > rgd[res])
   res = i;
 return res;
}

int Korali::Solver::CMAES::minIdx(const double *rgd, int len)
{
 int i, res;
 for(i=1, res=0; i<len; ++i)
  if(rgd[i] < rgd[res])
   res = i;
 return res;
}

/* dirty index sort */
void Korali::Solver::CMAES::sorted_index(const double *fitnessVector, int *iindex, int n)
{
 int i, j;
 for (i=1, iindex[0]=0; i<n; ++i) {
  for (j=i; j>0; --j) {
   if (fitnessVector[iindex[j-1]] < fitnessVector[i])
    break;
   iindex[j] = iindex[j-1]; /* shift up */
  }
  iindex[j] = i; /* insert i */
 }
}


double Korali::Solver::CMAES::doubleRangeMax(const double *rgd, int len)
{
 int i;
 double max = rgd[0];
 for (i = 1; i < len; ++i)
  max = (max < rgd[i]) ? rgd[i] : max;
 return max;
}

double Korali::Solver::CMAES::doubleRangeMin(const double *rgd, int len)
{
 int i;
 double min = rgd[0];
 for (i = 1; i < len; ++i)
  min = (min > rgd[i]) ? rgd[i] : min;
 return min;
}
