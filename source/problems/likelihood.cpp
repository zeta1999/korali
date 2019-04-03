#include "problems/likelihood.h"
#include "parameters/gaussian.h"
#include "parameters/uniform.h"
#include "conduits/base.h"

using json = nlohmann::json;

Korali::Problem::Likelihood::Likelihood(void (*modelFunction) (double*, double*), size_t seed) : Korali::Problem::Base::Base(seed)
{
 _referenceData = NULL;
 _referenceDataSize = 0;
 _modelFunction = modelFunction;
 _referenceDataSet = false;

 auto sigma = new Korali::Parameter::Uniform(0.0, +20.0);
 sigma->setName("Sigma");
 sigma->setBounds(0, +20.0);
 addParameter(sigma);
}

void Korali::Problem::Likelihood::setReferenceData(size_t nData, double* referenceData)
{
 _referenceDataSize = nData;
 _referenceData = referenceData;
 _referenceDataSet = true;
}

double Korali::Problem::Likelihood::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;

 double sigma = sample[0];
 double* parameters = &sample[1];
 double* fitnessData = _conduit->getFitnessArrayPointer();

 _modelFunction(parameters, fitnessData);

 return -Korali::Parameter::Gaussian::logLikelihood(sigma, _referenceDataSize, _referenceData, fitnessData);
}

json Korali::Problem::Likelihood::serialize()
{
 auto j = this->Korali::Problem::Base::serialize();
 j["Type"] = "Likelihood";

 return j;
}
