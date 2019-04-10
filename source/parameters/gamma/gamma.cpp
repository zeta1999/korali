#include "korali.h"

using namespace Korali::Parameter;

Gamma::Gamma() : _shape(1.), _rate(1.) {}

Gamma::Gamma(double shape, double rate) : Gamma(shape, rate, 0xC0FFEE){}

Gamma::Gamma(double shape, double rate, size_t seed) : _shape(shape), _rate(rate)
{
  this->Korali::Parameter::Base::initialize(seed);
  _aux = - gsl_sf_lngamma(_shape) - _shape*log(_rate);
}


double Gamma::getDensity(double x)
{
 return gsl_ran_gamma_pdf( x, _shape, _rate );
}

double Gamma::getDensityLog(double x)
{
 if(x < 0) return -INFINITY;
 return _aux + (_shape-1)*log(x) - x/_rate;
}

double Gamma::getRandomNumber()
{
 return gsl_ran_gamma(_range, _shape, _rate);
}


void Gamma::printDetails()
{
  printf("Gamma(%.3g,%.3g)", _shape, _rate);
}
