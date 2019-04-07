#include "korali.h"

using json = nlohmann::json;

Korali::Problem::Likelihood::Likelihood() : Korali::Problem::Base::Base()
{
	 _referenceData = NULL;
	 _referenceDataSize = 0;
}

void Korali::Problem::Likelihood::initialize()
{
	// auto sigma = new Korali::Parameter::Uniform(0.0, +20.0);
	// sigma->setName("Sigma");
	// sigma->setBounds(0, +20.0);
	// _k->addParameter(sigma);

	this->Korali::Problem::Base::initialize();
}

double Korali::Problem::Likelihood::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;

 double sigma = sample[0];
 double* parameters = &sample[1];
 double fitnessData[_referenceDataSize];

 _k->_modelMultiple(parameters, fitnessData);

 double likelihood = -Korali::Parameter::Gaussian::logLikelihood(sigma, _referenceDataSize, _referenceData, fitnessData);

 return likelihood;
}

json Korali::Problem::Likelihood::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();

 return js;
}

void Korali::Problem::Likelihood::setConfiguration(json js)
{
 this->Korali::Problem::Base::setConfiguration(js);

 if (js.find("Reference Data") != js.end()) if (js["Reference Data"].is_array())
 {
	 auto ref = js["Reference Data"];
	 _referenceDataSize = ref.size();
	 _referenceData = (double*) calloc (_referenceDataSize, sizeof(double));
	 for (int i = 0; i < _referenceDataSize; i++) _referenceData[i] = ref[i];
 }

 bool correctModel = false;
 if (js.find("Model") != js.end()) if (js["Model"].is_string())
 { if (js["Model"] == "Multiple") correctModel = true; }

 if (correctModel == false)
 {
	 fprintf(stderr, "[Korali] Error: Incorrect model for the Likelihood problem.\n");
	 exit(-1);
 }

 if (_referenceDataSize == 0)
 {
	 fprintf(stderr, "[Korali] Error: No Reference Data set for Likelihood.\n");
	 exit(-1);
 }

}
