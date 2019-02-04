#include "korali.h"
#include <chrono>

Korali::KoraliBase::KoraliBase(size_t dim, double (*fun) (double*, int), size_t seed = 0)
{
	_seed = seed;
	_dimCount = dim;

	gsl_rng_env_setup();
	_dims = new Dimension[dim];
	for (int i = 0; i < dim; i++) _dims[i].setSeed(seed++);
	_gaussianGenerator = new GaussianDistribution(0, 1, _seed++);
	_fitnessFunction = fun;

	_maxFitnessEvaluations = 900*(_dimCount+3)*(_dimCount+3);
	_maxGenerations = std::numeric_limits<size_t>::max();
}

Korali::Dimension* Korali::KoraliBase::getDimension(int dim) { return &_dims[dim]; }
Korali::Dimension* Korali::KoraliBase::operator[](int dim) { return getDimension(dim); }


double Korali::KoraliBase::getTotalDensity(double* x)
{
 double density = 1.0;
 for (int i = 0; i < _dimCount; i++) density *= _dims[i].getPriorDistribution()->getDensity(x[i]);
 return density;
}

double Korali::KoraliBase::getTotalDensityLog(double* x)
{
 double densityLog = 0.0;
 for (int i = 0; i < _dimCount; i++) densityLog += _dims[i].getPriorDistribution()->getDensityLog(x[i]);
 return densityLog;
}

void Korali::KoraliBase::setMaxFitnessEvaluations(size_t maxFitnessEvaluations) { _maxFitnessEvaluations = maxFitnessEvaluations; }
void Korali::KoraliBase::setMaxGenerations(size_t maxGenerations) { _maxGenerations = maxGenerations; }


