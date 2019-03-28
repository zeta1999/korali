#include "solvers/cmaes.h"
#include "conduits/base.h"
#include <chrono>

Korali::Solver::CMAESParameter::CMAESParameter()
{
	 _initialValue = 0.0;
	 _initialStdDev = 0.001;
	 _minStdDevChange = 0.0;
}

Korali::Solver::CMAES::CMAES(Korali::Problem::Base* problem) : Korali::Solver::Base::Base(problem)
{
 _maxFitnessEvaluations = std::numeric_limits<size_t>::max();

 _stopFitnessEvalThreshold = std::numeric_limits<double>::min();
 _stopFitnessDiffThreshold = 1e-12;
 _stopFitnessDiffHistoryThreshold = 1e-13;
 _stopMinDeltaX = 0.0;
 _stopMaxStdDevXFactor = 1e+03;
 _stopMaxTimePerEigendecomposition = 1.0;
 _stopMinFitness = -std::numeric_limits<double>::max();

 _mu = 0;
 _muType = "Logarithmic";
 _muCovariance = -1;
 _diagonalCovarianceMatrixEvalFrequency = 0;
 _sigmaCumulationFactor = -1;
 _dampFactor = -1;
 _cumulativeCovariance = -1;
 _covarianceMatrixLearningRate = -1;

 _gaussianGenerator = new Parameter::Gaussian(0.0, 1.0);
 _gaussianGenerator->initializeDistribution(problem->_seed + _problem->_parameterCount + 0xF0);

 for (size_t i = 0; i < N; i++)
 {
	_CMAESParameters.push_back(new CMAESParameter());
	_CMAESParameters[i]->setInitialValue((_problem->_parameters[i]->_lowerBound + _problem->_parameters[i]->_upperBound)*0.5);
	_CMAESParameterMap[_problem->_parameters[i]->_name] = _CMAESParameters[i];
 }
}

void Korali::Solver::CMAES::runSolver()
{
 printf("[Korali] Starting CMAES. Parameters: %ld, Seed: 0x%lX\n", N, _problem->_seed);

 initializeInternalVariables();

 auto startTime = std::chrono::system_clock::now();

 while( !checkTermination() )
 {
  auto t0 = std::chrono::system_clock::now();

  prepareGeneration();

   while (_finishedSamples < _sampleCount)
   {
    for (size_t i = 0; i < _sampleCount; i++) if (_initializedSample[i] == false)
    {
     _initializedSample[i] = true;
     _conduit->evaluateSample(i);
    }
    _conduit->checkProgress();
   }
  updateDistribution(_fitnessVector);

  auto t1 = std::chrono::system_clock::now();
  printf("[Korali] Gen %ld - Elapsed Time: %f, Convergence Rate: %7.2e\n", gen, std::chrono::duration<double>(t1-t0).count(), function_value_difference());
 }

 auto endTime = std::chrono::system_clock::now();

 printf("[Korali] Finished - Reason: %s\n", _terminationReason);
 printResults(); // Printing Solver results
 printf("[Korali] Total Elapsed Time: %fs\n", std::chrono::duration<double>(endTime-startTime).count());
}

void Korali::Solver::CMAES::processSample(size_t sampleId, double fitness)
{
 _fitnessVector[sampleId] = -fitness;
 _finishedSamples++;
}


bool Korali::Solver::CMAES::isFeasible(double *pop)
{
 for (size_t i = 0; i < _problem->_parameterCount; i++)
  if (pop[i] < _problem->_parameters[i]->_lowerBound || pop[i] > _problem->_parameters[i]->_upperBound)  return false;
 return true;
}


void Korali::Solver::CMAES::initializeInternalVariables()
{
 // Getting sample vector pointer
 _samplePopulation = _conduit->getSampleArrayPointer();

 // Initializing MU and its weights

 if (_mu == 0) _mu = _sampleCount;
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

 if(_mu < 1 || _mu > _sampleCount || (_mu == _sampleCount && _muWeights[0] == _muWeights[_mu-1]))
 { fprintf( stderr, "[Korali] Error: Invalid setting of Mu (%lu) and/or Lambda (%lu)\n", _mu, _sampleCount); exit(-1); }

 // Setting MU Covariance
 if (_muCovariance < 1) _muCovariance = _muEffective;

 // Checking Covariance Matrix Evaluation Frequency

 if (_diagonalCovarianceMatrixEvalFrequency == 0)  _diagonalCovarianceMatrixEvalFrequency = 2 + 100. * _problem->_parameterCount / sqrt((double)_sampleCount);
 if (_diagonalCovarianceMatrixEvalFrequency < 1)
 { fprintf( stderr, "[Korali] Error: Matrix covariance evaluation frequency is less than 1 (%lu)\n", _diagonalCovarianceMatrixEvalFrequency); exit(-1); }

 // Setting Sigma Cumulation Factor

 double tmpSigma = _sigmaCumulationFactor;
 if (tmpSigma > 0) _sigmaCumulationFactor *= (_muEffective + 2.0) / (_problem->_parameterCount + _muEffective + 3.0);
 if (tmpSigma <= 0 || tmpSigma >= 1)  _sigmaCumulationFactor = (_muEffective + 2.) / (_problem->_parameterCount + _muEffective + 3.0);

 // Setting Damping Factor

 if (_dampFactor < 0) _dampFactor = 1;
 _dampFactor = _dampFactor* (1 + 2*std::max(0.0, sqrt((_muEffective-1.0)/(_problem->_parameterCount+1.0)) - 1))  /* basic factor */
  * std::max(0.3, 1. - (double)_problem->_parameterCount / (1e-6+std::min(_maxGens, _maxFitnessEvaluations/_sampleCount)))
  + _sigmaCumulationFactor;             /* minor increment */

 // Setting Cumulative Covariance

 if (_cumulativeCovariance <= 0 || _cumulativeCovariance> 1)  _cumulativeCovariance = 4. / (_problem->_parameterCount + 4);

 // Set covariance Matrix Learning Rate

 double t1 = 2. / ((_problem->_parameterCount+1.4142)*(_problem->_parameterCount+1.4142));
 double t2 = (2.*_muEffective-1.) / ((_problem->_parameterCount+2.)*(_problem->_parameterCount+2.)+_muEffective);
 t2 = (t2 > 1) ? 1 : t2;
 t2 = (1./_muCovariance) * t1 + (1.-1./_muCovariance) * t2;

 double tmpCovarianceRate = _covarianceMatrixLearningRate;
 if (tmpCovarianceRate >= 0)   _covarianceMatrixLearningRate *= t2;
 if (tmpCovarianceRate < 0 || tmpCovarianceRate > 1)  _covarianceMatrixLearningRate = t2;

 // Setting eigensystem evaluation Frequency
   _covarianceEigensystemEvaluationFrequency = floor(1.0/(double)_covarianceMatrixLearningRate/((double)_problem->_parameterCount)/10.0);

 double trace = 0.0;
 for (size_t i = 0; i < N; ++i)   trace += _CMAESParameters[i]->_initialStdDev*_CMAESParameters[i]->_initialStdDev;
 //printf("Trace: %f\n", trace);
 sigma = sqrt(trace/N); /* _muEffective/(0.2*_muEffective+sqrt(N)) * sqrt(trace/N); */

 chiN = sqrt((double) N) * (1. - 1./(4.*N) + 1./(21.*N*N));
 flgEigensysIsUptodate = 1;

 double dtest = 1.0;
 for (; dtest && dtest < 1.1 * dtest; dtest *= 2.)  if (dtest == dtest + 1.)   break;
  dMaxSignifKond = dtest / 1000.; /* not sure whether this is really save, 100 does not work well enough */

  gen = 0;
  countevals = 0;
  state = 0;

  rgpc = (double*) calloc (sizeof(double), N);
  rgps = (double*) calloc (sizeof(double), N);
  rgdTmp = (double*) calloc (sizeof(double), N+1);
  rgBDz = (double*) calloc (sizeof(double), N);
  rgxmean = (double*) calloc (sizeof(double), N+2); rgxmean[0] = N; ++rgxmean;
  rgxold = (double*) calloc (sizeof(double), N+2); rgxold[0] = N; ++rgxold;
  rgxbestever = (double*) calloc (sizeof(double), N+3); rgxbestever[0] = N; ++rgxbestever;
  rgout = (double*) calloc (sizeof(double), N+2); rgout[0] = N; ++rgout;
  rgD = (double*) calloc (sizeof(double), N);
  C = (double**) calloc (sizeof(double*), N);
  B = (double**)calloc (sizeof(double*), N);
  rgFuncValue = (double*) calloc (sizeof(double), _sampleCount+1);
  rgFuncValue[0]=_sampleCount; ++rgFuncValue;
  arFuncValueHist = (double*) calloc (sizeof(double), 10+(int)ceil(3.*10.*N/_sampleCount)+1);
  arFuncValueHist[0] = (double)(10+(int)ceil(3.*10.*N/_sampleCount));
  arFuncValueHist++;

  for (size_t i = 0; i < N; ++i) {
   C[i] = (double*) calloc (sizeof(double), i+1);
   B[i] = (double*) calloc (sizeof(double), N);
  }
  index = (int *) calloc (sizeof(int*), _sampleCount);
  for (size_t i = 0; i < _sampleCount; ++i)  index[i] = i; /* should not be necessary */
  curBest = (double *) calloc (sizeof(double), _sampleCount);

  /* Initialize newed space  */

 for (size_t i = 0; i < N; ++i)
  for (size_t j = 0; j < i; ++j)
   C[i][j] = B[i][j] = B[j][i] = 0.;

 for (size_t i = 0; i < N; ++i)
 {
  B[i][i] = 1.;
  C[i][i] = rgD[i] = _CMAESParameters[i]->_initialStdDev * sqrt(N / trace);
  C[i][i] *= C[i][i];
  rgpc[i] = rgps[i] = 0.;
 }

 minEW = doubleRangeMin(rgD, N); minEW = minEW * minEW;
 maxEW = doubleRangeMax(rgD, N); maxEW = maxEW * maxEW;

 maxdiagC=C[0][0]; for(size_t i=1;i<N;++i) if(maxdiagC<C[i][i]) maxdiagC=C[i][i];
 mindiagC=C[0][0]; for(size_t i=1;i<N;++i) if(mindiagC>C[i][i]) mindiagC=C[i][i];

 /* set rgxmean */
 for (size_t i = 0; i < N; ++i)
 {
   if(_CMAESParameters[i]->_initialValue < _problem->_parameters[i]->_lowerBound || _CMAESParameters[i]->_initialValue > _problem->_parameters[i]->_upperBound)
    {
  	 _CMAESParameters[i]->_initialValue = (_problem->_parameters[i]->_upperBound+_problem->_parameters[i]->_lowerBound)*0.5;
    //fprintf(stderr,"[Korali] Warning: Initial Value (%.4f) for \'%s\' is out of bounds (%.4f-%.4f).\n", _CMAESParameters[i]->_initialValue, _problem->_parameters[i]->_name.c_str(), _problem->_parameters[i]->_lowerBound, _problem->_parameters[i]->_upperBound);
    //fprintf(stderr,"[Korali] This may cause the engine to deadlock trying to find a good candidate.\n");
    //fprintf(stderr,"[Korali] Use e.g., parameter.setInitialValue(%.4f) to set a new initial value.\n", (_problem->_parameters[i]->_upperBound+_problem->_parameters[i]->_lowerBound)*0.5);
    }

   rgxmean[i] = rgxold[i] = _CMAESParameters[i]->_initialValue;
 }

 _initializedSample = (bool*) calloc (_sampleCount, sizeof(bool));
 _fitnessVector = (double*) calloc (sizeof(double), _sampleCount);
}

void Korali::Solver::CMAES::prepareGeneration()
{
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));

 /* calculate eigensystem  */
 if (!flgEigensysIsUptodate) {
  if (!flgdiag)
   updateEigensystem(0);
  else {
   for (size_t i = 0; i < N; ++i)
    rgD[i] = sqrt(C[i][i]);
		 minEW = doubleRangeMin(rgD, N) * doubleRangeMin(rgD, N);
		 maxEW = doubleRangeMax(rgD, N) * doubleRangeMin(rgD, N);
		 flgEigensysIsUptodate = 1;
  }
 }

 /* treat minimal standard deviations and numeric problems */
 for (size_t i = 0; i < N; ++i)
  while (sigma * sqrt(C[i][i]) < _CMAESParameters[i]->_minStdDevChange)
   sigma *= exp(0.05+_sigmaCumulationFactor/_dampFactor);

 for (size_t iNk = 0; iNk < _sampleCount; ++iNk)
 { /* generate scaled random vector (D * z) */
  for (size_t i = 0; i < N; ++i)
  {
   if (flgdiag) _samplePopulation[iNk * N + i] = rgxmean[i] + sigma * rgD[i] * _gaussianGenerator->getRandomNumber();
   else rgdTmp[i] = rgD[i] * _gaussianGenerator->getRandomNumber();
  }

  if (!flgdiag)
   for (size_t i = 0; i < N; ++i) {
  	double sum = 0.0;
    for (size_t j = 0; j < N; ++j)
     sum += B[i][j] * rgdTmp[j];
    _samplePopulation[iNk * N + i] = rgxmean[i] + sigma * sum;
   }
 }

 if(state == 3 || gen == 0)   ++gen;
 state = 1;

 for(size_t i = 0; i < _sampleCount; ++i) while( !isFeasible(&_samplePopulation[i*N] )) reSampleSingle(i);

 _finishedSamples = 0;
 for (size_t i = 0; i < _sampleCount; i++) _initializedSample[i] = false;
}


void Korali::Solver::CMAES::reSampleSingle(size_t idx)
{
 double *rgx;

 if (idx < 0 || idx >= _sampleCount)  fprintf(stderr, "[Korali] Error: reSampleSingle(): Population member \n");
 rgx = &_samplePopulation[idx*N];

 for (size_t i = 0; i < N; ++i)  rgdTmp[i] = rgD[i] * _gaussianGenerator->getRandomNumber();

 /* add mutation (sigma * B * (D*z)) */
 for (size_t i = 0; i < N; ++i) {
	double sum = 0.0;
  for (size_t j = 0; j < N; ++j)
   sum += B[i][j] * rgdTmp[j];
  rgx[i] = rgxmean[i] + sigma * sum;
 }
}


void Korali::Solver::CMAES::updateDistribution(const double *fitnessVector)
{
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));

 if(state == 3)
  fprintf(stderr, "[Korali] Error: updateDistribution(): You need to call SamplePopulation() before update can take place.");
 if(fitnessVector == NULL)
  fprintf(stderr, "[Korali] Error: updateDistribution(): Fitness function value array input is missing.");

 if(state == 1)  /* function values are delivered here */
  countevals += _sampleCount;
 else
  fprintf(stderr, "[Korali] Error: updateDistribution(): unexpected state");

 /* assign function values */
 for (size_t i = 0; i < _sampleCount; i++) curBest[i] = rgFuncValue[i] = fitnessVector[i];

 /* Generate index */
 sorted_index(fitnessVector, index, _sampleCount);

 /* Test if function values are identical, escape flat fitness */
 if (rgFuncValue[index[0]] ==
   rgFuncValue[index[(int)_sampleCount/2]]) {
  sigma *= exp(0.2+_sigmaCumulationFactor/_dampFactor);
  fprintf(stderr, "[Korali] Error: Warning: sigma increased due to equal function values.\n");
  fprintf(stderr, "[Korali] Reconsider the formulation of the objective function\n");
 }

 /* update function value history */
   for(size_t i = (size_t)*(arFuncValueHist-1)-1; i > 0; --i)
     arFuncValueHist[i] = arFuncValueHist[i-1];
   arFuncValueHist[0] = fitnessVector[index[0]];

 /* update xbestever */
 if ((rgxbestever[N] > curBest[index[0]] || gen == 1))
 {
  for (size_t i = 0; i < N; ++i) rgxbestever[i] = _samplePopulation[index[0]*N + i];
  rgxbestever[N] = curBest[index[0]];
  rgxbestever[N+1] = countevals;
 }

 /* calculate rgxmean and rgBDz~N(0,C) */
 for (size_t i = 0; i < N; ++i) {
  rgxold[i] = rgxmean[i];
  rgxmean[i] = 0.;
  for (size_t iNk = 0; iNk < _mu; ++iNk)
   rgxmean[i] += _muWeights[iNk] * _samplePopulation[index[iNk]*N + i];
  rgBDz[i] = sqrt(_muEffective)*(rgxmean[i] - rgxold[i])/sigma;
 }

 double sum = 0.0;
 /* calculate z := D^(-1) * B^(-1) * rgBDz into rgdTmp */
 for (size_t i = 0; i < N; ++i) {
  if (!flgdiag)
   for (size_t j = 0, sum = 0.; j < N; ++j)
    sum += B[j][i] * rgBDz[j];
  else
   sum = rgBDz[i];
  rgdTmp[i] = sum / rgD[i];
 }

 /* cumulation for sigma (ps) using B*z */
 for (size_t i = 0; i < N; ++i) {
  if (!flgdiag)
   for (size_t j = 0, sum = 0.; j < N; ++j)
    sum += B[i][j] * rgdTmp[j];
  else
   sum = rgdTmp[i];
  rgps[i] = (1. - _sigmaCumulationFactor) * rgps[i] +
   sqrt(_sigmaCumulationFactor * (2. - _sigmaCumulationFactor)) * sum;
 }

 /* calculate norm(ps)^2 */
 double psxps = 0.0;
 for (size_t i = 0; i < N; ++i)  psxps += rgps[i] * rgps[i];

 /* cumulation for covariance matrix (pc) using B*D*z~N(0,C) */
 int hsig = sqrt(psxps) / sqrt(1. - pow(1.-_sigmaCumulationFactor, 2*gen)) / chiN  < 1.4 + 2./(N+1);

 for (size_t i = 0; i < N; ++i)
  rgpc[i] = (1. - _cumulativeCovariance) * rgpc[i] +  hsig * sqrt(_cumulativeCovariance * (2. - _cumulativeCovariance)) * rgBDz[i];

 /* update of C  */
 adaptC2(hsig);

 /* update of sigma */
 sigma *= exp(((sqrt(psxps)/chiN)-1.)*_sigmaCumulationFactor/_dampFactor);

 state = 3;
}


void Korali::Solver::CMAES::adaptC2(int hsig)
{
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));

 if (_covarianceMatrixLearningRate != 0.0)
 {
  /* definitions for speeding up inner-most loop */
  double ccov1 = std::min(_covarianceMatrixLearningRate * (1./_muCovariance) * (flgdiag ? (N+1.5) / 3. : 1.), 1.);
  double ccovmu = std::min(_covarianceMatrixLearningRate * (1-1./_muCovariance)* (flgdiag ? (N+1.5) / 3. : 1.), 1.-ccov1);
  double sigmasquare = sigma * sigma;

  flgEigensysIsUptodate = 0;

  /* update covariance matrix */
  for (size_t i = 0; i < N; ++i)
   for (size_t j = flgdiag ? i : 0; j <= i; ++j) {
		 C[i][j] = (1 - ccov1 - ccovmu) * C[i][j] + ccov1	* (rgpc[i] * rgpc[j]	+ (1-hsig)*_cumulativeCovariance*(2.-_cumulativeCovariance) * C[i][j]);
		 for (size_t k = 0; k < _mu; ++k)	C[i][j] += ccovmu * _muWeights[k] * (_samplePopulation[index[k]*N + i] - rgxold[i]) * (_samplePopulation[index[k]*N + j] - rgxold[j]) / sigmasquare;
   }

  /* update maximal and minimal diagonal value */
  maxdiagC = mindiagC = C[0][0];
  for (size_t i = 1; i < N; ++i) {
	 if (maxdiagC < C[i][i]) maxdiagC = C[i][i];
	 else if (mindiagC > C[i][i])  mindiagC = C[i][i];
  }
 } /* if ccov... */
}


void Korali::Solver::CMAES::printResults()
{

 for (size_t i = 0; i < N; i++)
 {
  printf("[Korali] Parameter \'%s\' Value: %f\n", _problem->_parameters[i]->_name.c_str(), rgxbestever[i]);
 }

 if(_verbose)
 {
  printf("(%lu,%lu)-CMA-ES(mu_eff=%.1f), dimension=%lu, diagonalIterations=%lu, randomSeed=%lu\n", _mu, _sampleCount, _muEffective,  _problem->_parameterCount, _diagonalCovarianceMatrixEvalFrequency,  _problem->_seed);
  printf(" N %ld\n", N);
  printf(" seed %lu\n", _problem->_seed);
  printf("function evaluations %.0f\n", countevals);
  printf("function value f(x)=%g\n", curBest[index[0]]);
  printf("maximal standard deviation %g\n", sigma*sqrt(maxdiagC));
  printf("minimal standard deviation %g\n", sigma*sqrt(mindiagC));
  printf("sigma %g\n", sigma);
  printf("axisratio %g\n", doubleRangeMax(rgD, N)/doubleRangeMin(rgD, N));
  printf("xbestever found after %.0f evaluations, function value %.10g\n", rgxbestever[N+1], rgxbestever[N]);
  for(size_t i=0; i<N; ++i) printf(" %.12g%c", rgxbestever[i], (i%5==4||i==N-1)?'\n':' ');
  printf("xbest (of last generation, function value %g)\n", curBest[index[0]]);
  for(size_t i=0; i<N; ++i) printf(" %12g%c", _samplePopulation[index[0]*N + i],(i%5==4||i==N-1)?'\n':' ');
  printf("rgxmean \n");
  for(size_t i=0; i<N; ++i) printf(" %12g%c", rgxmean[i], (i%5==4||i==N-1)?'\n':' ');
  printf("Standard deviation of coordinate axes (sigma*sqrt(diag(C)))\n");
  for(size_t i=0; i<N; ++i) printf(" %12g%c", sigma*sqrt(C[i][i]), (i%5==4||i==N-1)?'\n':' ');
  printf("Main axis lengths of mutation ellipsoid (sigma*diag(D))\n");
  for(size_t i = 0; i < N; ++i) rgdTmp[i] = rgD[i];
  for(size_t i=0; i<N; ++i) printf(" %12g%c", sigma*rgdTmp[N-1-i],(i%5==4||i==N-1)?'\n':' ');
  printf("Longest axis (b_i where d_ii=max(diag(D))\n");
  for(size_t i=0; i<N; ++i) printf(" %12g%c", B[i][maxIdx(rgD, N)], (i%5==4||i==N-1)?'\n':' ');
  printf("Shortest axis (b_i where d_ii=max(diag(D))\n");
  for(size_t i=0; i<N; ++i) printf(" %12g%c", B[i][minIdx(rgD, N)], (i%5==4||i==N-1)?'\n':' ');
 }
}

double Korali::Solver::CMAES::function_value_difference()
{
 return std::max(doubleRangeMax(arFuncValueHist, (int)std::min((double)gen,*(arFuncValueHist-1))),
   doubleRangeMax(rgFuncValue, _sampleCount)) -
  std::min(doubleRangeMin(arFuncValueHist, (int)std::min((double)gen, *(arFuncValueHist-1))),
      doubleRangeMin(rgFuncValue, _sampleCount));
}

bool Korali::Solver::CMAES::checkTermination()
{
 double range, fac;
 int flgdiag = ((_diagonalCovarianceMatrixEvalFrequency== 1) || (_diagonalCovarianceMatrixEvalFrequency>= gen));
 bool terminate = false;

 /* function value reached */
 if ((gen > 1 || state > 1) &&   rgFuncValue[index[0]] <= _stopMinFitness)
 {
  terminate = true;
  sprintf(_terminationReason, "Fitness Value (%7.2e) < (%7.2e)",  rgFuncValue[index[0]], _stopMinFitness);
 }

 /* TolFun */
 range = function_value_difference();

 if (gen > 0 && range <= _stopFitnessDiffThreshold) {
  terminate = true;
  sprintf(_terminationReason, "Function value differences (%7.2e) < (%7.2e)",  range, _stopFitnessDiffThreshold);
 }

 /* TolFunHist */
 if (gen > *(arFuncValueHist-1)) {
  range = doubleRangeMax(arFuncValueHist, (int)*(arFuncValueHist-1))
   - doubleRangeMin(arFuncValueHist, (int)*(arFuncValueHist-1));
  if (range <= _stopFitnessDiffHistoryThreshold)
  {
   terminate = true;
   sprintf(_terminationReason, "Function value changes (%7.2e) < (%7.2e)", range, _stopFitnessDiffHistoryThreshold);
  }
 }

 size_t cTemp = 0;
 for(size_t i=0; i<N; ++i) {
  cTemp += (sigma * sqrt(C[i][i]) < _stopMinDeltaX) ? 1 : 0;
  cTemp += (sigma * rgpc[i] < _stopMinDeltaX) ? 1 : 0;
 }

 if (cTemp == 2*N) {
  terminate = true;
  sprintf(_terminationReason, "Object variable changes < %7.2e", _stopMinDeltaX);
 }

 /* TolUpX */
 // Re Check this
// size_t tolPos;
// for(tolPos = 0; tolPos < N; tolPos++) {
//  if (sigma * sqrt(C[tolPos][tolPos]) > _stopMaxStdDevXFactor * _CMAESParameters[tolPos]->_initialStdDev)
//   break;
// }
//
// if (tolPos < N) {
//  terminate = true;
//  sprintf(_terminationReason, "Standard deviation increased by > %7.2e. Try a larger initial stddev.", _stopMaxStdDevXFactor);
// }

 /* Condition of C greater than dMaxSignifKond */
 if (maxEW >= minEW * dMaxSignifKond) {
  terminate = true;
  sprintf(_terminationReason, "Maximal condition number %7.2e reached.", dMaxSignifKond);
 } /* if */

  size_t iAchse = 0;
  size_t iKoo = 0;
  if (!flgdiag) {
  for (iAchse = 0; iAchse < N; ++iAchse)
  {
   fac = 0.1 * sigma * rgD[iAchse];
   for (iKoo = 0; iKoo < N; ++iKoo){
    if (rgxmean[iKoo] != rgxmean[iKoo] + fac * B[iKoo][iAchse])
     break;
   }
   if (iKoo == N)
   {
    terminate = true;
    if(_verbose) sprintf(_terminationReason, "Standard deviation 0.1*%7.2e in principal axis %ld without effect.", fac/0.1, iAchse);
    break;
   } /* if (iKoo == N) */
  } /* for iAchse    */
 } /* if flgdiag */

 /* Component of rgxmean is not changed anymore */
 for (iKoo = 0; iKoo < N; ++iKoo)
 {
  if (rgxmean[iKoo] == rgxmean[iKoo] +
    0.2*sigma*sqrt(C[iKoo][iKoo]))
  {
   /* C[iKoo][iKoo] *= (1 + _covarianceMatrixLearningRate); */
   /* flg = 1; */
   terminate = true;
   if(_verbose) sprintf(_terminationReason, "Standard deviation 0.2*%7.2e in coordinate %ld without effect.", sigma*sqrt(C[iKoo][iKoo]), iKoo);
   break;
  }

 } /* for iKoo */

 if(countevals >= _maxFitnessEvaluations)
 {
  terminate = true;
  sprintf(_terminationReason, "Conducted %.0f function evaluations >= (%lu).", countevals, _maxFitnessEvaluations); }

 if(gen >= _maxGens)
 {
  terminate = true;
  sprintf(_terminationReason, "Maximum number of Generations reached (%lu).", _maxGens);
 }

 return terminate;
}

void Korali::Solver::CMAES::updateEigensystem(int flgforce)
{
 if(flgforce == 0) if (flgEigensysIsUptodate == 1) return;

 eigen( N, C, rgD, B);

 /* find largest and smallest eigenvalue, they are supposed to be sorted anyway */
 minEW = doubleRangeMin(rgD, N);
 maxEW = doubleRangeMax(rgD, N);

 for (size_t i = 0; i < N; ++i)  rgD[i] = sqrt(rgD[i]);

 flgEigensysIsUptodate = 1;
}

void Korali::Solver::CMAES::eigen( int size,  double **C, double *diag, double **Q)
{
 double* data = (double*) malloc (sizeof(double) * size * size);

 for (int i = 0; i <  size; i++)
 for (int j = 0; j <= i; j++)
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

 for (int i = 0; i < size; i++)
 {
  gsl_vector_view evec_i = gsl_matrix_column (evec, i);
  for (int j = 0; j < size; j++) Q[j][i] =  -gsl_vector_get (&evec_i.vector, j);
 }

 for (int i = 0; i < size; i++) diag[i] = gsl_vector_get (eval, i);

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
