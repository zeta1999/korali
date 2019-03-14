#include "parameter.h"

Korali::Parameter::Parameter()
{
	_name = "";
 _initialX = 0.0;
 _initialStdDev = 0.1;

	_lowerBound = -100;
	_upperBound = +100;
  _a = -100;
  _b = +100;
  _type = "Uniform";
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
}

// Verify that distribution type is correctly set
void Korali::Parameter::initializePriorDistribution(int seed)
{
  if (_type == "Uniform")    { _prior = new UniformDistribution(_a, _b, seed);  return; }
  if (_type == "Gaussian")   { _prior = new GaussianDistribution(_a, _b, seed); return; }
  if (_type == "Exponential"){ _prior = new ExponentialDistribution(_a, seed);  return; }
  if (_type == "Gamma")      { _prior = new GammaDistribution(_a, _b, seed);    return; }

  fprintf(stderr, "[Korali] Error: Unrecognized Distribution: %s. Defaulting to Uniform.\n", _type.c_str());
  _prior = new UniformDistribution(_a, _b, seed);
}

void Korali::Parameter::setBounds(double lowerBound, double upperBound)
{
	_lowerBound = lowerBound;
	_upperBound = upperBound;
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
