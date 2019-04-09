#include "korali.h"

using json = nlohmann::json;

json Korali::Solver::CMAES::getConfiguration()
{
 auto js = this->Korali::Solver::Base::getConfiguration();

 js["Engine"] = "CMA-ES";
 js["Mu"] = _mu;
 js["MuType"] = _muType;
 js["diagonalCovarianceMatrixEvalFrequency"] = _diagonalCovarianceMatrixEvalFrequency;
 js["covarianceEigensystemEvaluationFrequency"] = _covarianceEigensystemEvaluationFrequency;
 js["muCovariance"] = _muCovariance;
 js["sigmaCumulationFactor"] = _sigmaCumulationFactor;
 js["dampFactor"] = _dampFactor;
 js["cumulativeCovariance"] = _cumulativeCovariance;
 js["covarianceMatrixLearningRate"] = _covarianceMatrixLearningRate;

 js["Termination Criteria"]["MaxFitnessEvaluations"] = _maxFitnessEvaluations ;
 js["Termination Criteria"]["stopFitnessEvalThreshold"] = _stopFitnessEvalThreshold ;
 js["Termination Criteria"]["stopFitnessDiffThreshold"] = _stopFitnessDiffThreshold ;
 js["Termination Criteria"]["stopMinDeltaX"] = _stopMinDeltaX;
 js["Termination Criteria"]["stopMinFitness"] = _stopMinFitness;

 return js;
}

void Korali::Solver::CMAES::setConfiguration(json js)
{
 _maxFitnessEvaluations = std::numeric_limits<size_t>::max();

 _stopFitnessEvalThreshold = std::numeric_limits<double>::min();
 _stopFitnessDiffThreshold = 1e-12;
 _stopMinDeltaX = 0.0;
 _stopMinFitness = -std::numeric_limits<double>::max();

 _mu = 0;
 _muType = "Logarithmic";
 _muCovariance = -1;
 _diagonalCovarianceMatrixEvalFrequency = 0;
 _sigmaCumulationFactor = -1;
 _dampFactor = -1;
 _cumulativeCovariance = -1;
 _covarianceMatrixLearningRate = -1;
 _maxGens = 200;
 _currentGeneration = 0;

 _terminationReason[0] = '\0';

  this->Korali::Solver::Base::setConfiguration(js);

  if (js.find("currentGeneration") != js.end()) if (js["currentGeneration"].is_number())
  { _currentGeneration = js["currentGeneration"]; js.erase("currentGeneration"); }

  if (js.find("Lambda") != js.end()) if (js["Lambda"].is_number())
  { _s = js["Lambda"]; js.erase("Lambda"); }

  if (js.find("dampFactor") != js.end()) if (js["dampFactor"].is_number())
  { _dampFactor = js["dampFactor"]; js.erase("dampFactor"); }

  if (js.find("sigmaCumulationFactor") != js.end()) if (js["sigmaCumulationFactor"].is_number())
  { _sigmaCumulationFactor = js["sigmaCumulationFactor"]; js.erase("sigmaCumulationFactor"); }

  if (js.find("cumulativeCovariance") != js.end()) if (js["cumulativeCovariance"].is_number())
  { _cumulativeCovariance = js["cumulativeCovariance"]; js.erase("cumulativeCovariance"); }

  if (js.find("Mu") != js.end())
  {
    json mu = js["Mu"];
    if (mu.find("Value") != mu.end()) if (mu["Value"].is_number())
    { _mu = mu["Value"]; mu.erase("Value"); }
   if (mu.find("Type") != mu.end()) if (mu["Type"].is_string())
    { _muType = mu["Type"]; mu.erase("Type"); }
   if (mu.find("Covariance") != mu.end()) if (mu["Covariance"].is_number())
   { _muCovariance = mu["Covariance"]; mu.erase("Covariance"); }
  }

  if (js.find("CovarianceMatrix") != js.end())
  {
    json cov = js["CovarianceMatrix"];
   if (cov.find("DiagonalEvaluationFrequency") != cov.end()) if (cov["DiagonalEvaluationFrequency"].is_number())
    { _diagonalCovarianceMatrixEvalFrequency = cov["DiagonalEvaluationFrequency"]; cov.erase("DiagonalEvaluationFrequency"); }
   if (cov.find("EigenEvaluationFrequency") != cov.end()) if (cov["EigenEvaluationFrequency"].is_number())
   { _diagonalCovarianceMatrixEvalFrequency = cov["EigenEvaluationFrequency"]; cov.erase("EigenEvaluationFrequency"); }
   if (cov.find("LearningRate") != cov.end()) if (cov["LearningRate"].is_number())
   { _covarianceMatrixLearningRate = cov["LearningRate"]; cov.erase("LearningRate"); }
  }

  if (js.find("TerminationCriteria") != js.end())
  {
    json term = js["TerminationCriteria"];
   if (term.find("MaxFitnessEvaluations") != term.end()) if (term["MaxFitnessEvaluations"].is_number())
    { _maxFitnessEvaluations = term["MaxFitnessEvaluations"]; term.erase("MaxFitnessEvaluations"); }
   if (term.find("stopFitnessEvalThreshold") != term.end()) if (term["stopFitnessEvalThreshold"].is_number())
   { _stopFitnessEvalThreshold = term["stopFitnessEvalThreshold"]; term.erase("stopFitnessEvalThreshold"); }
   if (term.find("stopFitnessDiffThreshold") != term.end()) if (term["stopFitnessDiffThreshold"].is_number())
   { _stopFitnessDiffThreshold = term["stopFitnessDiffThreshold"]; term.erase("stopFitnessDiffThreshold"); }
   if (term.find("stopMinDeltaX") != term.end()) if (term["stopMinDeltaX"].is_number())
   { _stopMinDeltaX = term["stopMinDeltaX"]; term.erase("stopMinDeltaX"); }
   if (term.find("stopMinFitness") != term.end()) if (term["stopMinFitness"].is_number())
   { _stopMinFitness = term["stopMinFitness"]; term.erase("stopMinFitness"); }
  }
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

 for (int i = 0; i < _mu; i++) js["MuWeights"] += _muWeights[i];
 for (int i = 0; i < _k->N; i++) js["CurrentMeanVector"] += rgxmean[i];
 for (int i = 0; i < _k->N; i++) js["PreviousMeanVector"] += rgxold[i];
 for (int i = 0; i < _k->N; i++) js["BestEverVector"] += rgxbestever[i];
 for (int i = 0; i < _k->N; i++) js["CurrentBestVector"] += curBest[i];
 for (int i = 0; i < _k->N; i++) js["Index"] += index[i];
 for (int i = 0; i < _k->N; i++) js["AxisLengths"] += rgD[i];
 for (int i = 0; i < _k->N; i++) js["CumulativeCovariance"] += rgpc[i];
 for (int i = 0; i < _k->N; i++) js["FunctionValues"] += rgFuncValue[i];

 for (int i = 0; i < _s; i++) for (int j = 0; j < _k->N; j++) js["Samples"][i] += _samplePopulation[i*_k->N + j];
 for (int i = 0; i < _s; i++) js["SampleFitness"] += _fitnessVector[i];

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

  for (int i = 0; i < _mu; i++) _muWeights[i] = js["MuWeights"][i];
  for (int i = 0; i < _k->N; i++) rgxmean[i]      = js["CurrentMeanVector"][i];
  for (int i = 0; i < _k->N; i++) rgxold[i]       = js["PreviousMeanVector"][i];
  for (int i = 0; i < _k->N; i++) rgxbestever[i]  = js["BestEverVector"][i];
  for (int i = 0; i < _k->N; i++) curBest[i]      = js["CurrentBestVector"][i];
  for (int i = 0; i < _k->N; i++) index[i]        = js["Index"][i];
  for (int i = 0; i < _k->N; i++) rgD[i]          = js["AxisLengths"][i];
  for (int i = 0; i < _k->N; i++) rgpc[i]         = js["CumulativeCovariance"][i];
  for (int i = 0; i < _k->N; i++) rgFuncValue[i]  = js["FunctionValues"][i];
  for (int i = 0; i < _k->N; i++) for (int j = 0; j < _k->N; j++) C[i][j] = js["CovarianceMatrix"][i][j];
  for (int i = 0; i < _k->N; i++) for (int j = 0; j < _k->N; j++) B[i][j] = js["EigenMatrix"][i][j];

  for (int i = 0; i < _s; i++) for (int j = 0; j < _k->N; j++) _samplePopulation[i*_k->N + j] = js["Samples"][i][j];
  for (int i = 0; i < _s; i++) _fitnessVector[i] = js["SampleFitness"][i];
}
