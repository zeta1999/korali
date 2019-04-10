#include "korali.h"

using namespace Korali::Parameter;

Exponential::Exponential() : _loc(0.), _mean(1.) {}

Exponential::Exponential(double mean) : Exponential(0.0,mean){}

Exponential::Exponential(double loc, double mean) : Exponential(loc, mean, 0xC0FFEE){}

Exponential::Exponential(double loc, double mean, size_t seed) : _loc(loc), _mean(mean)
{
  this->Korali::Parameter::Base::initialize(seed);
}




double Exponential::getDensity(double x)
{
 return  gsl_ran_exponential_pdf(x-_loc, _mean);
}

double Exponential::getDensityLog(double x)
{
 if (x-_loc < 0) return -INFINITY;
 return - log(_mean) - (x-_loc)/_mean;
}

double Exponential::getRandomNumber()
{
 return _loc + gsl_ran_exponential(_range, _mean);
}


void Exponential::printDetails()
{
  printf("Exponential(%.3g,%.3g)", _loc, _mean);
}
