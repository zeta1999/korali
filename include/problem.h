#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "parameter.h"

namespace Korali
{

class Problem
{
  public:

	Problem(size_t seed = 0);

	void addParameter(Parameter p);
  virtual double evaluateFitness(double* sample) = 0;
  virtual bool evaluateSettings(char* errorCode) = 0;

  size_t _parameterCount;
	size_t _seed;

  std::vector<Parameter> _parameters;

  double getPriorsLogProbabilityDensity(double *x);
  double getPriorsProbabilityDensity(double *x);
};

class Likelihood : public Problem
{
  public:

	size_t _nData;
	double* _referenceData;
	double* (*_modelFunction) (double*);

	bool _modelDataSet;
	bool _referenceDataSet;

  void setReferenceData(size_t nData, double* referenceData);
	Likelihood(double* (*modelFunction) (double*), size_t seed = -1);
	double evaluateFitness(double* sample);
	bool evaluateSettings(char* errorCode);
};

class Posterior : public Likelihood
{
  public:

	Posterior(double* (*modelFunction) (double*), size_t seed = -1);
	bool evaluateSettings(char* errorCode);
	double evaluateFitness(double* sample);
};

class DirectEvaluation : public Problem
{
  public:
	double (*_modelFunction) (double*);
	DirectEvaluation(double (*modelFunction) (double*), size_t seed = -1);
	double evaluateFitness(double* sample);
	bool evaluateSettings(char* errorCode);
};



} // namespace Korali


#endif // _PROBLEM_H_
