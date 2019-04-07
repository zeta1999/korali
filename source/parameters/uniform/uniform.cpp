#include "korali.h"

Korali::Parameter::Uniform::Uniform() : Korali::Parameter::Base::Base()
{
	_min = -1.0;
	_max = +1.0;
};

double Korali::Parameter::Uniform::getDensity(double x)
{
 return gsl_ran_flat_pdf(x, _min, _max);
}

double Korali::Parameter::Uniform::getDensityLog(double x)
{
 if (x >= _min && x <= _max)
  return -log(_max-_min);
 return -INFINITY;
}

double Korali::Parameter::Uniform::getRandomNumber()
{
 return gsl_ran_flat(_range, _min, _max);
}
