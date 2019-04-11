#include "korali.h"

using json = nlohmann::json;

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
 this->Korali::Solver::Base::setConfiguration(js);

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
  this->Korali::Solver::Base::setState(js);

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
