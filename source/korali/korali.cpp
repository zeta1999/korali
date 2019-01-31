#include "korali.h"

Korali::KoraliBase* kb;

Korali::KoraliBase::KoraliBase(int dim, double (*fun) (double*, int), int seed = 0)
{
	_dimCount = dim;

	gsl_rng_env_setup();
	_dims = new Dimension[dim];
	for (int i = 0; i < dim; i++) _dims[i].setSeed(seed + i);

	_fitfunction = fun;

	_maxFitnessEvaluations = 900*(dim+3)*(dim+3);
	_maxGenerations = std::numeric_limits<size_t>::max();
	_lambda = 128;
	setMu(64);
	_muCovariance = _muEffective;

	_stopFitnessEvalThreshold = std::numeric_limits<double>::min();
	_stopFitnessDiffThreshold = 1e-12;
	_stopFitnessDiffHistoryThreshold = 1e-13;
	_stopMinDeltaX = 0.0;
	_stopMaxStdDevXFactor = 1e+03;
	_stopMaxTimePerEigendecomposition = 1.0;

	kb = this;
}

Korali::Dimension* Korali::KoraliBase::getDimension(int dim) { return &_dims[dim]; }
Korali::Dimension* Korali::KoraliBase::operator[](int dim) { return getDimension(dim); }

void Korali::KoraliBase::setMaxFitnessEvaluations(size_t maxFitnessEvaluations) { _maxFitnessEvaluations = maxFitnessEvaluations; }
void Korali::KoraliBase::setMaxGenerations(size_t maxGenerations) { _maxGenerations = maxGenerations; }
void Korali::KoraliBase::setLambda(size_t lambda) { _lambda = lambda; }

void Korali::KoraliBase::setStopFitnessEvalThreshold(double stopFitnessEvalThreshold) { _stopFitnessEvalThreshold = stopFitnessEvalThreshold; }
void Korali::KoraliBase::setStopFitnessDiffThreshold(double stopFitnessDiffThreshold) { _stopFitnessDiffThreshold = stopFitnessDiffThreshold; }
void Korali::KoraliBase::setStopFitnessDiffHistoryThreshold(double stopFitnessDiffHistoryThreshold) { _stopFitnessDiffHistoryThreshold = stopFitnessDiffHistoryThreshold; }
void Korali::KoraliBase::setStopMinDeltaX(double stopMinDeltaX) { _stopMinDeltaX = stopMinDeltaX; }
void Korali::KoraliBase::setStopMaxStdDevXFactor(double stopMaxStdDevXFactor) { _stopMaxStdDevXFactor = stopMaxStdDevXFactor; }
void Korali::KoraliBase::setStopMaxTimePerEigenDecomposition(double stopMaxTimePerEigendecomposition) { _stopMaxTimePerEigendecomposition = stopMaxTimePerEigendecomposition; }

void Korali::KoraliBase::setMu(size_t mu, std::string type)
{
	_mu = mu;
	_muWeights = new double[_mu];

  if (type == "LinearDecreasing") for (int i = 0; i < _mu; i++)  _muWeights[i] = _mu - i;
  if (type == "Equal")  for (int i = 0; i < _mu; i++)   _muWeights[i] = 1;
  if (type == "Logarithmic") for (int i = 0; i < _mu; i++)  _muWeights[i] = log(_mu+1.)-log(i+1.);

  /* normalize weights vector and set mueff */
  double s1 = 0.0;
  double s2 = 0.0;

  for (int i=0; i < _mu; i++)
  {
   s1 += _muWeights[i];
   s2 += _muWeights[i]*_muWeights[i];
  }

  _muEffective = s1*s1/s2;

  for (int i = 0; i < _mu; i++) _muWeights[i] /= s1;

  if(_mu < 1 || _mu > _lambda || (_mu == _lambda && _muWeights[0] == _muWeights[_mu-1]))
  { fprintf( stderr, "[Korali] Error: Invalid setting of Mu (%d) and/or Lambda (%d)\n", _mu, _lambda); exit(-1); }

}

void Korali::KoraliBase::setMuCovariance(double muCovariance) { if (muCovariance < 1) _muCovariance = _muEffective; else _muCovariance = muCovariance; }

void Korali::KoraliBase::Run()
{
	//  if (_dimArray.size() != dimCount) { fprintf( stderr, "[Korali] Error: Prior has a different dimension count (%d) than the problem (%d). \n", _dimArray.size(), dimCount); exit(-1); }

}
