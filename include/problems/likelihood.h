#ifndef _LIKELIHOOD_H_
#define _LIKELIHOOD_H_

#include "problems/base.h"

namespace Korali
{

class Likelihood : public BaseProblem
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


#endif // _LIKELIHOOD_H_
