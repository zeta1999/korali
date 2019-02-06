#include "prior.h"

Korali::Prior::Prior()
{
 _prior = new UniformDistribution(-1.0, 1.0, 0);
 _lowerBound = -std::numeric_limits<double>::max();
 _upperBound = std::numeric_limits<double>::max();
 _initialX = 0.0;
 _initialStdDev = 0.0;
 _lowerBound = std::numeric_limits<double>::min();

 _seed = 0;
}

void Korali::Prior::setPriorDistribution(std::string type, double a, double b)
{
  if (type == "Uniform") _prior = new UniformDistribution(a, b, _seed);
  if (type == "Gaussian") _prior = new GaussianDistribution(a, b, _seed);
  if (type == "Exponential") _prior = new ExponentialDistribution(a, _seed);
  if (type == "Gamma") _prior = new GammaDistribution(a, b, _seed);
}

Korali::Distribution* Korali::Prior::getPriorDistribution() { return _prior; }
void Korali::Prior::setBounds(double lowerBound, double upperBound) { _lowerBound = lowerBound; _upperBound = upperBound;}
void Korali::Prior::setInitialX(double initialX) { _initialX = initialX; }
void Korali::Prior::setInitialStdDev(double initialStdDev) { _initialStdDev = initialStdDev; }
void Korali::Prior::setSeed(int seed) { _seed = seed; }
void Korali::Prior::setMinStdDevChange(double minStdDevChange) { _minStdDevChange = minStdDevChange; }
