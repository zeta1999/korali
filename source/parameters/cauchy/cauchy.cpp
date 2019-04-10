#include "korali.h"

using namespace Korali::Parameter;

Cauchy::Cauchy() : _loc(0.), _scale(1.) {}

Cauchy::Cauchy(double scale) : Cauchy(0.0,scale){}

Cauchy::Cauchy(double loc, double scale) : Cauchy(loc, scale, 0xC0FFEE){}

Cauchy::Cauchy(double loc, double scale, size_t seed) : _loc(loc), _scale(scale)
{
  this->Korali::Parameter::Base::initialize(seed);
  _aux   = -gsl_sf_log( _scale * M_PI );
}



double Cauchy::getDensity(double x)
{
 printf("Cauchy(%.3g,%.3g)", _loc, _scale);
 return gsl_ran_cauchy_pdf( x-_loc, _scale );
}

double Cauchy::getDensityLog(double x)
{
 printf("Cauchy(%.3g,%.3g)", _loc, _scale);
 return  _aux - gsl_sf_log( 1. + gsl_sf_pow_int((x-_loc)/_scale,2) );
}

double Cauchy::getRandomNumber()
{
 return _loc + gsl_ran_cauchy(_range, _scale);
}


void Cauchy::printDetails()
{
  printf("Cauchy(%.3g,%.3g)", _loc, _scale);
}
