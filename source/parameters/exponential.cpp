#include "parameters/exponential.h"

Korali::Parameter::Exponential::Exponential(double mean) : Korali::Parameter::Exponential::Exponential("NoNameExponential", mean){}
Korali::Parameter::Exponential::Exponential(std::string name, double mean) : Korali::Parameter::Base::Base(name)
{
 _mean = mean;
}

double Korali::Parameter::Exponential::getDensity(double x)
{
 return  gsl_ran_exponential_pdf(x, _mean);
}

double Korali::Parameter::Exponential::getDensityLog(double x)
{
 if (x < 0) return -INFINITY;
 return - log(_mean) - x/_mean;
}

double Korali::Parameter::Exponential::getRandomNumber()
{
 return gsl_ran_exponential(_range, _mean);
}
