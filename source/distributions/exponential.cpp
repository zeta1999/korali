#include "distributions/exponential.h"

Korali::Exponential::Exponential(double mean) : Korali::BaseDistribution::BaseDistribution()
{
	_mean = mean;
}

double Korali::Exponential::getDensity(double x) { return  gsl_ran_exponential_pdf(x, _mean); }
double Korali::Exponential::getDensityLog(double x) { if (x < 0) return -INFINITY; 	return - log(_mean) - x/_mean;}
double Korali::Exponential::getRandomNumber() { return gsl_ran_exponential(_range, _mean);}
