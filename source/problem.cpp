#include "problem.h"
#include <time.h>
#include <stdlib.h>

Korali::ProblemBase::ProblemBase(void (*fun) (double*, int, double*), size_t seed)
{
	_seed = seed;
	if (_seed == 0) _seed = clock();
	_fitnessFunction = fun;
	_dimCount = 0;
  gsl_rng_env_setup();
}

Korali::Minimizer::Minimizer(  void (*fun) (double*, int, double*), size_t seed) : Korali::ProblemBase::ProblemBase(fun, seed) { _engine = new Korali::KoraliCMAES(this, MPI_COMM_WORLD); }
Korali::Maximizer::Maximizer(  void (*fun) (double*, int, double*), size_t seed) : Korali::ProblemBase::ProblemBase(fun, seed) { _engine = new Korali::KoraliCMAES(this, MPI_COMM_WORLD); }

Korali::Posterior::Posterior(void (*fun) (double*, int, double*), size_t seed) : Korali::ProblemBase::ProblemBase(fun, seed)
{
	_engine = new Korali::KoraliCMAES(this, MPI_COMM_WORLD);

	Korali::Parameter sigma;
	sigma.setPriorDistribution("Uniform", 0, std::numeric_limits<double>::max());
	sigma.setBounds(0, std::numeric_limits<double>::max());
	sigma.setInitialX(1);
	sigma.setInitialStdDev(1);

	addParameter(sigma);
}

void Korali::ProblemBase::addParameter(Parameter p)
{
	_parameters.push_back(p);
	p.setSeed(_seed++);
	_dimCount = _parameters.size();
}

double Korali::ProblemBase::getTotalDensity(double* x)
{
 double density = 1.0;
 for (int i = 0; i < _parameters.size(); i++) density *= _parameters[i].getPriorDistribution()->getDensity(x[i]);
 return density;
}

double Korali::ProblemBase::getTotalDensityLog(double* x)
{
 double densityLog = 0.0;
 for (int i = 0; i < _dimCount; i++) densityLog += _parameters[i].getPriorDistribution()->getDensityLog(x[i]);
 return densityLog;
}

double Korali::Minimizer::evaluateFitness(double* sample)
{
	double result;
  _fitnessFunction(sample, _dimCount, &result);
  return result;
}

double Korali::Maximizer::evaluateFitness(double* sample)
{
	double result;
  _fitnessFunction(sample, _dimCount, &result);
  return -result;
}

double Korali::Posterior::evaluateFitness(double* sample)
{
	double* results = (double*) calloc (sizeof(double), _refData.size());
	_fitnessFunction(sample+1, _dimCount-1, results);

  double sigma2 = sample[0]*sample[0];
  double ssn = 0.0;
  int nData = _refData.size();

  for(int i = 0; i < nData; i++) { double diff = _refData[i] - results[i]; ssn += diff*diff; }

  double res = 0.5* (nData*log(2*M_PI) + nData*log(sigma2) + ssn/sigma2);

  printf("Sigma: %.2f - Res: %f\n", sample[0], res);
	return res;
}

void Korali::Posterior::addReferenceData(double ref)
{
	_refData.push_back(ref);
}
