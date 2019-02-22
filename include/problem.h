#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "parameter.h"
#include "cmaes.h"

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
};

class Likelihood : public Problem
{
  public:

	size_t _nData;
	double* _referenceData;
	void* _modelData;
	double* (*_modelFunction) (double*, void*);

	bool _modelDataSet;
	bool _referenceDataSet;

  void setModelData(void* modelData);
  void setReferenceData(size_t nData, double* referenceData);
	Likelihood(double* (*modelFunction) (double*, void*), size_t seed = 0);
	double evaluateFitness(double* sample);
	bool evaluateSettings(char* errorCode);
};

class Posterior : public Likelihood
{
  public:

	Posterior(double* (*modelFunction) (double*, void*), size_t seed = 0);
	bool evaluateSettings(char* errorCode);
	double evaluateFitness(double* sample);
};

class Minimization : public Problem
{
  public:
	double (*_modelFunction) (double*);
	Minimization(double (*modelFunction) (double*), size_t seed = 0);
	double evaluateFitness(double* sample);
	bool evaluateSettings(char* errorCode);
};


} // namespace Korali


#endif // _PROBLEM_H_
