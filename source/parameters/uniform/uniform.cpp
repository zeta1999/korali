#include "korali.h"

Korali::Parameter::Uniform::Uniform() : Korali::Parameter::Base::Base()
{
};

double Korali::Parameter::Uniform::getDensity(double x)
{
 return gsl_ran_flat_pdf(x, _lowerBound, _upperBound);
}

double Korali::Parameter::Uniform::getDensityLog(double x)
{
 if (x >= _lowerBound && x <= _upperBound)
  return -log(_upperBound-_lowerBound);
 return -INFINITY;
}

double Korali::Parameter::Uniform::getRandomNumber()
{
 return gsl_ran_flat(_range, _lowerBound, _upperBound);
}
