#include "distributions/uniform.h"

Korali::Uniform::Uniform(double min, double max) : Korali::BaseDistribution::BaseDistribution()
{
	_min = min;
	_max = max;
	 _lowerBound = min;
	 _upperBound = max;
}

double Korali::Uniform::getDensity(double x) { return gsl_ran_flat_pdf(x, _min, _max);}
double Korali::Uniform::getDensityLog(double x) { if (x >= _min && x <= _max) return -log(_max-_min); return -INFINITY; }
double Korali::Uniform::getRandomNumber() { return gsl_ran_flat(_range, _min, _max); }
