#include "korali.h"

using namespace Korali::Parameter;

Uniform::Uniform() : Korali::Parameter::Base::Base(){}

Uniform::Uniform(double lowerBound, double upperBound) : Uniform(lowerBound, upperBound, 0xC0FFEE){}

Uniform::Uniform(double lowerBound, double upperBound, size_t seed) : Korali::Parameter::Base::Base()
{
   _lowerBound = lowerBound;
   _upperBound = upperBound;
  this->Korali::Parameter::Base::initialize(seed);
  _aux = -gsl_sf_log(_upperBound-_lowerBound);
}





double Uniform::getDensity(double x)
{
 return gsl_ran_flat_pdf(x, _lowerBound, _upperBound);
}

double Uniform::getDensityLog(double x)
{
 if (x >= _lowerBound && x <= _upperBound)
  return _aux;
 return -GSL_NEGINF;
}

double Uniform::getRandomNumber()
{
 return gsl_ran_flat(_range, _lowerBound, _upperBound);
}


void Uniform::printDetails()
{
  printf("Uniform(%.3g,%.3g)", _lowerBound, _upperBound);
}
