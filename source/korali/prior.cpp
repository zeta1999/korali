#include "prior.h"

void Korali::Prior::addDimension(Distribution* d)
{
 _dimArray.push_back(d);
}

void Korali::Prior::initialize(int dimCount)
{
 _randomNumber = (double*) calloc (sizeof(double), _dimArray.size());
}

double Korali::Prior::getDensity(double* x)
{
 double density = 1.0;
 for (int i = 0; i < _dimArray.size(); i++) density *= _dimArray[i]->getDensity(x[i]);
 return density;
}

double Korali::Prior::getDensityLog(double* x)
{
 double densityLog = 0.0;
 for (int i = 0; i < _dimArray.size(); i++) densityLog += _dimArray[i]->getDensityLog(x[i]);
 return densityLog;
}

double* Korali::Prior::getRandomNumber()
{
 for (int i = 0; i < _dimArray.size(); i++) _randomNumber[i] = _dimArray[i]->getRandomNumber();
 return _randomNumber;
}
