#ifndef _KORALI_LIKELIHOOD_H_
#define _KORALI_LIKELIHOOD_H_

#include "problems/base.h"

namespace Korali::Problem
{

class Likelihood : public Korali::Problem::Base
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
};

} // namespace Korali


#endif // _KORALI_LIKELIHOOD_H_
