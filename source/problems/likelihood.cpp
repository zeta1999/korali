#include "problems/likelihood.h"
#include <stdlib.h>
#include <chrono>

Korali::Likelihood::Likelihood(double* (*modelFunction) (double*), size_t seed) : Korali::BaseProblem::BaseProblem(seed)
{
	_referenceData = NULL;
	_nData = 0;
	_modelFunction = modelFunction;
	_referenceDataSet = false;

	Korali::Parameter sigma("Sigma");
	sigma.setInitialX(10.0);
	sigma.setBounds(0, 20.0);
	sigma.setPriorDistribution("Uniform", 0.0, +20.0);
	addParameter(sigma);
}

void Korali::Likelihood::setReferenceData(size_t nData, double* referenceData)
{
	_nData = nData;
	_referenceData = referenceData;
	_referenceDataSet = true;
}

double Korali::Likelihood::evaluateFitness(double* sample)
{
	double sigma = sample[0];
	double* parameters = &sample[1];
  double* measuredData = _modelFunction(parameters);

	return -Korali::GaussianDistribution::logLikelihood(sigma, _nData, _referenceData, measuredData);
}
