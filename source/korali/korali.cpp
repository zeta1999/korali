#include "korali.h"

Korali::KoraliBase* k;

//  if (_dimArray.size() != dimCount) { fprintf( stderr, "[Korali] Error: Prior has a different dimension count (%d) than the problem (%d). \n", _dimArray.size(), dimCount); exit(-1); }

Korali::KoraliBase::KoraliBase(int dim, double (*fun) (double*, int), int seed = 0)
{
	gsl_rng_env_setup();
	_dims = new Dimension[dim];
	for (int i = 0; i < dim; i++) _dims[i].setSeed(seed + i);

	_fitfunction = fun;

	_maxFitnessEvaluations = 900*(dim+3)*(dim+3);
	_maxGenerations = std::numeric_limits<size_t>::max();
	_lambda = 128;

	_stopFitnessEvalThreshold = std::numeric_limits<double>::min();
	_stopFitnessDiffThreshold = 1e-12;
	_stopFitnessDiffHistoryThreshold = 1e-13;
	_stopMinDeltaX = 0.0;
	_stopMaxStdDevXFactor = 1e+03;
	_stopMaxTimePerEigendecomposition = 1.0;

	k = this;
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
