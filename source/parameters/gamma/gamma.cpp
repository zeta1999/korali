#include "korali.h"

double Korali::Parameter::Gamma::getDensity(double x)
{
 return gsl_ran_gamma_pdf( x, _shape, _rate );
}

double Korali::Parameter::Gamma::getDensityLog(double x)
{
 if(x < 0) return -INFINITY;
 return - gsl_sf_lngamma(_shape) - _shape*log(_rate) + (_shape-1)*log(x) - x/_rate;
}

double Korali::Parameter::Gamma::getRandomNumber()
{
 return gsl_ran_gamma(_range, _shape, _rate);
}

