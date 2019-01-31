#include "dimension.h"

Korali::Dimension::Dimension()
{
 _prior = new UniformDistribution(-1.0, 1.0, 0);
 _lowerBound = std::numeric_limits<double>::min();
 _upperBound = std::numeric_limits<double>::max();
 _initialX = 0.0;
 _typicalX = 0.0;
 _initialStdDev = 0.0;
 _seed = 0;
}

void Korali::Dimension::setPriorDistribution(std::string type, double a, double b)
{
  if (type == "Uniform") _prior = new UniformDistribution(a, b, _seed);
  if (type == "Gaussian") _prior = new GaussianDistribution(a, b, _seed);
  if (type == "Exponential") _prior = new ExponentialDistribution(a, _seed);
  if (type == "Gamma") _prior = new GammaDistribution(a, b, _seed);
}

Korali::Distribution* Korali::Dimension::getPriorDistribution() { return _prior; }
void Korali::Dimension::setBounds(double lowerBound, double upperBound) { _lowerBound = lowerBound; _upperBound = upperBound;}
void Korali::Dimension::setInitialX(double initialX) { _initialX = initialX; }
void Korali::Dimension::setTypicalX(double typicalX) { _typicalX = typicalX; }
void Korali::Dimension::setInitialStdDev(double initialStdDev) { _initialStdDev = initialStdDev; }
void Korali::Dimension::setSeed(int seed) { _seed = seed; }
