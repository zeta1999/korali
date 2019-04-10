#include "korali.h"

using namespace Korali::Parameter;

Laplace::Laplace() : _mean(0.), _width(1.) {}

Laplace::Laplace(double mean, double width) : Laplace(mean, width, 0xC0FFEE){}

Laplace::Laplace(double mean, double width, size_t seed) : _mean(mean), _width(width)
{
  this->Korali::Parameter::Base::initialize(seed);
   _aux   = - gsl_sf_log(2.*_width);
}

double Laplace::getDensity(double x)
{
 return gsl_ran_laplace_pdf( x-_mean, _width );
}

double Laplace::getDensityLog(double x)
{
 return  _aux - fabs(x-_mean)/_width;
}

double Laplace::getRandomNumber()
{
 return _mean + gsl_ran_laplace(_range, _width);
}


void Laplace::printDetails()
{
  printf("Laplace(%.3g,%.3g)", _mean, _width);
}
