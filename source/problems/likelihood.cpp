#include "problems/likelihood.h"
#include "parameters/gaussian.h"
#include "parameters/uniform.h"

Korali::Likelihood::Likelihood(double* (*modelFunction) (double*), size_t seed) : Korali::BaseProblem::BaseProblem(seed)
{
	_referenceData = NULL;
	_nData = 0;
	_modelFunction = modelFunction;
	_referenceDataSet = false;

	auto sigma = new Korali::Parameter::Uniform(0.0, +20.0);
	sigma->setName("Sigma");
	sigma->setInitialX(+10.0);
	sigma->setBounds(0, +20.0);
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

	return -Korali::Parameter::Gaussian::logLikelihood(sigma, _nData, _referenceData, measuredData);
}
