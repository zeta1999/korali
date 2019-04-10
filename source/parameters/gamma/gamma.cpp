#include "korali.h"

using namespace Korali::Parameter;

Gamma::Gamma() : _shape(1.), _scale(1.) {}

Gamma::Gamma(double shape, double scale) : Gamma(shape, scale, 0xC0FFEE){}

Gamma::Gamma(double shape, double scale, size_t seed) : _shape(shape), _scale(scale)
{
  this->Korali::Parameter::Base::initialize(seed);
  _aux = - gsl_sf_lngamma(_shape) - _shape*log(_scale);
}


double Gamma::getDensity(double x)
{
 return gsl_ran_gamma_pdf( x, _shape, _scale );
}

double Gamma::getDensityLog(double x)
{
 if(x < 0) return -INFINITY;
 return _aux + (_shape-1)*log(x) - x/_scale;
}

double Gamma::getRandomNumber()
{
 return gsl_ran_gamma(_range, _shape, _scale);
}


void Gamma::printDetails()
{
  printf("Gamma(%.3g,%.3g)", _shape, _scale);
}
