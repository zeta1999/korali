#include "korali.h"

using json = nlohmann::json;

Korali::Solver::CMAES::CMAES(nlohmann::json& js) : Korali::Solver::Base::Base(js)
{
 setConfiguration(js);

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

Korali::Solver::CMAES::~CMAES()
{

}


json Korali::Solver::CMAES::getConfiguration()
{
 auto js = this->Korali::Solver::Base::getConfiguration();

 js["Engine"] = "CMA-ES";

 js["Lambda"] = _s;
 js["Current Generation"] = _currentGeneration;
 js["Sigma Cumulation Factor"] = _sigmaCumulationFactor;
 js["Damp Factor"] = _dampFactor;

 js["Mu"]["Value"] =  _mu;
 js["Mu"]["Type"]  =  _muType;
 js["Mu"]["Covariance"] = _muCovariance;

 js["Covariance Matrix"]["Eigenvalue Evaluation Frequency"] = _covarianceEigenEvalFreq;
 js["Covariance Matrix"]["Cumulative Covariance"] = _cumulativeCovariance;
 js["Covariance Matrix"]["Learning Rate"] = _covarianceMatrixLearningRate;

 js["Termination Criteria"]["Max Generations"] = _covarianceMatrixLearningRate;
 js["Termination Criteria"]["Min Fitness"] = _stopMinFitness;
 js["Termination Criteria"]["Max Model Evaluations"] = _maxFitnessEvaluations;
 js["Termination Criteria"]["Fitness Eval Threshold"] = _stopFitnessEvalThreshold;
 js["Termination Criteria"]["Fitness Diff Threshold"] = _stopFitnessDiffThreshold;
 js["Termination Criteria"]["Min DeltaX"] = _stopMinDeltaX;
 js["Termination Criteria"]["Min Fitness"] = _stopMinFitness;

 return js;
}

void Korali::Solver::CMAES::setConfiguration(json& js)
{
 _s                             = consume(js, { "Lambda" }, KORALI_NUMBER);
 _currentGeneration             = consume(js, { "Current Generation" }, KORALI_NUMBER, std::to_string(0));
 _sigmaCumulationFactor         = consume(js, { "Sigma Cumulation Factor" }, KORALI_NUMBER, std::to_string(-1));
 _dampFactor                    = consume(js, { "Damp Factor" }, KORALI_NUMBER, std::to_string(-1));

 _mu                            = consume(js, { "Mu", "Value" }, KORALI_NUMBER, std::to_string(ceil(_s / 2)));
 _muType                        = consume(js, { "Mu", "Type" }, KORALI_STRING, "Logarithmic");
 _muCovariance                  = consume(js, { "Mu", "Covariance" }, KORALI_NUMBER, std::to_string(-1));

 _covarianceEigenEvalFreq       = consume(js, { "Covariance Matrix", "Eigenvalue Evaluation Frequency" }, KORALI_NUMBER, std::to_string(0));
 _cumulativeCovariance          = consume(js, { "Covariance Matrix", "Cumulative Covariance" }, KORALI_NUMBER, std::to_string(-1));
 _covarianceMatrixLearningRate  = consume(js, { "Covariance Matrix", "Learning Rate" }, KORALI_NUMBER, std::to_string(-1));

 _maxGens                       = consume(js, { "Termination Criteria", "Max Generations" }, KORALI_NUMBER, std::to_string(200));
 _stopMinFitness                = consume(js, { "Termination Criteria", "Min Fitness" }, KORALI_NUMBER, std::to_string(-std::numeric_limits<double>::max()));
 _maxFitnessEvaluations         = consume(js, { "Termination Criteria", "Max Model Evaluations" }, KORALI_NUMBER, std::to_string(std::numeric_limits<size_t>::max()));
 _stopFitnessEvalThreshold      = consume(js, { "Termination Criteria", "Fitness Eval Threshold" }, KORALI_NUMBER, std::to_string(std::numeric_limits<double>::min()));
 _stopFitnessDiffThreshold      = consume(js, { "Termination Criteria", "Fitness Diff Threshold" }, KORALI_NUMBER, std::to_string(1e-9));
 _stopMinDeltaX                 = consume(js, { "Termination Criteria", "Min DeltaX" }, KORALI_NUMBER, std::to_string(0.0));
 _stopMinFitness                = consume(js, { "Termination Criteria", "Min Fitness" }, KORALI_NUMBER, std::to_string(-std::numeric_limits<double>::max()));
}

json Korali::Solver::CMAES::getState()
{
 auto js = this->Korali::Solver::Base::getState();

 js["MuEffective"] = _muEffective;
 js["Sigma"] = sigma;
 js["CurrentBest"] = currentBest;
 js["CurrentFunctionValue"] = currentFunctionValue;
 js["prevFunctionValue"] = prevFunctionValue;
 js["MaxDiagonalCovariance"] = maxdiagC;
 js["MinDiagonalCovariance"] = mindiagC;
 js["MaxEigenvalue"] = maxEW;
 js["MinEigenvalue"] = minEW;
 js["EigenSystemUpToDate"] = flgEigensysIsUptodate;
 js["EvaluationCount"] = countevals;

 for (size_t i = 0; i < _mu; i++) js["MuWeights"] += _muWeights[i];
 for (size_t i = 0; i < _k->N; i++) js["CurrentMeanVector"] += rgxmean[i];
 for (size_t i = 0; i < _k->N; i++) js["PreviousMeanVector"] += rgxold[i];
 for (size_t i = 0; i < _k->N; i++) js["BestEverVector"] += rgxbestever[i];
 for (size_t i = 0; i < _k->N; i++) js["CurrentBestVector"] += curBest[i];
 for (size_t i = 0; i < _k->N; i++) js["Index"] += index[i];
 for (size_t i = 0; i < _k->N; i++) js["AxisLengths"] += rgD[i];
 for (size_t i = 0; i < _k->N; i++) js["CumulativeCovariance"] += rgpc[i];
 for (size_t i = 0; i < _k->N; i++) js["FunctionValues"] += rgFuncValue[i];

 for (size_t i = 0; i < _s; i++) for (size_t j = 0; j < _k->N; j++) js["Samples"][i] += _samplePopulation[i*_k->N + j];
 for (size_t i = 0; i < _s; i++) js["SampleFitness"] += _fitnessVector[i];

 return js;
}

void Korali::Solver::CMAES::setState(json js)
{
  _muEffective          = js["MuEffective"];
  sigma                 = js["Sigma"];
  currentBest           = js["CurrentBest"];
  currentFunctionValue  = js["CurrentFunctionValue"];
  prevFunctionValue     = js["prevFunctionValue"];
  maxdiagC              = js["MaxDiagonalCovariance"];
  mindiagC              = js["MinDiagonalCovariance"];
  maxEW                 = js["MaxEigenvalue"];
  minEW                 = js["MinEigenvalue"] ;
  flgEigensysIsUptodate = js["EigenSystemUpToDate"];
  countevals            = js["EvaluationCount"];

  for (size_t i = 0; i < _mu; i++) _muWeights[i] = js["MuWeights"][i];
  for (size_t i = 0; i < _k->N; i++) rgxmean[i]      = js["CurrentMeanVector"][i];
  for (size_t i = 0; i < _k->N; i++) rgxold[i]       = js["PreviousMeanVector"][i];
  for (size_t i = 0; i < _k->N; i++) rgxbestever[i]  = js["BestEverVector"][i];
  for (size_t i = 0; i < _k->N; i++) curBest[i]      = js["CurrentBestVector"][i];
  for (size_t i = 0; i < _k->N; i++) index[i]        = js["Index"][i];
  for (size_t i = 0; i < _k->N; i++) rgD[i]          = js["AxisLengths"][i];
  for (size_t i = 0; i < _k->N; i++) rgpc[i]         = js["CumulativeCovariance"][i];
  for (size_t i = 0; i < _k->N; i++) rgFuncValue[i]  = js["FunctionValues"][i];
  for (size_t i = 0; i < _k->N; i++) for (size_t j = 0; j < _k->N; j++) C[i][j] = js["CovarianceMatrix"][i][j];
  for (size_t i = 0; i < _k->N; i++) for (size_t j = 0; j < _k->N; j++) B[i][j] = js["EigenMatrix"][i][j];

  for (size_t i = 0; i < _s; i++) for (size_t j = 0; j < _k->N; j++) _samplePopulation[i*_k->N + j] = js["Samples"][i][j];
  for (size_t i = 0; i < _s; i++) _fitnessVector[i] = js["SampleFitness"][i];
}
