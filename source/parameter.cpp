#include "parameter.h"

Korali::Parameter::Parameter()
{
	_name = "";
 _initialX = 0.0;
 _initialStdDev = 0.1;
 _minStdDevChange = 0.0;
	_lowerBound = 0.0;
	_upperBound = 0.0;
  _a = 0.0;
  _b = 0.0;
  _type = "Undefined";
  _seed = 0;
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
void Korali::Parameter::initialize(int seed)
{
	_seed = seed;
  if (_type == "Uniform")    { _prior = new UniformDistribution(_a, _b, seed);  return; }
  if (_type == "Gaussian")   { _prior = new GaussianDistribution(_a, _b, seed); return; }
  if (_type == "Exponential"){ _prior = new ExponentialDistribution(_a, seed);  return; }
  if (_type == "Gamma")      { _prior = new GammaDistribution(_a, _b, seed);    return; }
  _type = "Unrecognized";
}

void Korali::Parameter::setBounds(double lowerBound, double upperBound)
{
	_lowerBound = lowerBound;
	_upperBound = upperBound;
}

void Korali::Parameter::checkDistribution()
{
  if (_type == "Undefined" || _type == "Unrecognized")
  {
  	 fprintf(stderr, "[Korali] Warning: Undefined or Unrecognized Prior Distribution for %s.\n", _name.c_str());
  	 fprintf(stderr, "[Korali] Defaulting to Uniform[%f,%f], Seed = %lu\n",_lowerBound, _upperBound, _seed);
  	 setPriorDistribution("Uniform", _lowerBound, _upperBound);
  	 initialize(_seed);
  }
}

double Korali::Parameter::getRandomNumber()
{
	checkDistribution();

  double x = _prior->getRandomNumber();

  while (x < _lowerBound || x > _upperBound) x = _prior->getRandomNumber();

  return x;
}

double Korali::Parameter::getDensityLog(double x)
{
	checkDistribution();
  return _prior->getDensityLog(x);
}

double Korali::Parameter::getDensity(double x)
{
	checkDistribution();
  return _prior->getDensity(x);
}

void Korali::Parameter::setName(std::string name) { _name = name; }
void Korali::Parameter::setInitialX(double initialX) { _initialX = initialX; }
void Korali::Parameter::setInitialStdDev(double initialStdDev) { _initialStdDev = initialStdDev; }
void Korali::Parameter::setMinStdDevChange(double minStdDevChange) { _minStdDevChange = minStdDevChange; }
