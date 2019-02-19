#include "parameter.h"

Korali::Parameter::Parameter()
{
 _prior = new UniformDistribution(-1.0, 1.0, 0);
 _lowerBound = -std::numeric_limits<double>::max();
 _upperBound = std::numeric_limits<double>::max();
 _initialX = 0.0;
 _initialStdDev = 0.0;
 _lowerBound = std::numeric_limits<double>::min();

 _seed = 0;
}

void Korali::Parameter::setPriorDistribution(std::string type, double a, double b)
{
  if (type == "Uniform") _prior = new UniformDistribution(a, b, _seed);
  if (type == "Gaussian") _prior = new GaussianDistribution(a, b, _seed);
  if (type == "Exponential") _prior = new ExponentialDistribution(a, _seed);
  if (type == "Gamma") _prior = new GammaDistribution(a, b, _seed);
}

Korali::Distribution* Korali::Parameter::getPriorDistribution() { return _prior; }
void Korali::Parameter::setBounds(double lowerBound, double upperBound) { _lowerBound = lowerBound; _upperBound = upperBound;}
void Korali::Parameter::setInitialX(double initialX) { _initialX = initialX; }
void Korali::Parameter::setInitialStdDev(double initialStdDev) { _initialStdDev = initialStdDev; }
void Korali::Parameter::setSeed(int seed) { _seed = seed; }
void Korali::Parameter::setMinStdDevChange(double minStdDevChange) { _minStdDevChange = minStdDevChange; }
