#include "parameter.h"

Korali::Parameter::Parameter()
{
	_priorSet  = false;
	_boundsSet = false;
	_name = "";
 _initialX = 0.0;
 _initialStdDev = 0.1;
 _lowerBound = std::numeric_limits<double>::min();
}

Korali::Parameter::Parameter(std::string name) : Korali::Parameter::Parameter()
{
	_name = name;
}

void Korali::Parameter::setPriorDistribution(std::string type, double a, double b)
{
	_type = type;
	_a = a;
	_b = b;
  _priorSet = true;
}

// Verify that distribution type is correctly set
void Korali::Parameter::initializePriorDistribution(int seed)
{
  if (_type == "Uniform")     _prior = new UniformDistribution(_a, _b, seed);
  if (_type == "Gaussian")    _prior = new GaussianDistribution(_a, _b, seed);
  if (_type == "Exponential") _prior = new ExponentialDistribution(_a, seed);
  if (_type == "Gamma")       _prior = new GammaDistribution(_a, _b, seed);
}

void Korali::Parameter::setBounds(double lowerBound, double upperBound)
{
	_lowerBound = lowerBound;
	_upperBound = upperBound;
	_boundsSet = true;
}

double Korali::Parameter::getRandomNumber()
{
  double x = _prior->getRandomNumber();

  while (x < _lowerBound || x > _upperBound) x = _prior->getRandomNumber();

  return x;
}

double Korali::Parameter::getDensityLog(double x)
{
  return _prior->getDensityLog(x);
}

double Korali::Parameter::getDensity(double x)
{
  return _prior->getDensity(x);
}

void Korali::Parameter::setName(std::string name) { _name = name; }
void Korali::Parameter::setInitialX(double initialX) { _initialX = initialX; }
void Korali::Parameter::setInitialStdDev(double initialStdDev) { _initialStdDev = initialStdDev; }
void Korali::Parameter::setMinStdDevChange(double minStdDevChange) { _minStdDevChange = minStdDevChange; }
