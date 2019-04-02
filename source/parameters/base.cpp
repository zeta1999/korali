#include "parameters/base.h"

Korali::Parameter::Base::Base(std::string name)
{
 _name = name;
 _lowerBound = 0.0;
 _upperBound = -1.0;
 _initialValue = 0.5;
 _initialStdDev = 0.001;
 _minStdDevChange = 0.0;
}

void Korali::Parameter::Base::initializeDistribution(int seed)
{
 _range = gsl_rng_alloc (gsl_rng_default);
 gsl_rng_set(_range, seed);
}

void Korali::Parameter::Base::checkBounds()
{
 if (_upperBound <= _lowerBound)
 {
  fprintf(stderr, "[Korali] Warning: Undefined or invalid lower/upper bounds for %s.\n", _name.c_str());
  fprintf(stderr, "[Korali] Tip: Use parameter.setBounds(lower,upper); to define bounds.\n");
  fprintf(stderr, "[Korali] Defaulting to [-100.0; 100.0]\n");
  _lowerBound = -100.0;
  _upperBound =  100.0;
 }

 if(_initialValue < _lowerBound || _initialValue > _upperBound)
  fprintf(stderr,"[Korali] Warning: Initial Value (%.4f) for \'%s\' is out of bounds (%.4f-%.4f).\n", _initialValue, _name.c_str(), _lowerBound, _upperBound);
}

void Korali::Parameter::Base::setBounds(double lowerBound, double upperBound)
{
 _lowerBound = lowerBound;
 _upperBound = upperBound;
 _initialValue = (lowerBound+upperBound)*0.5;
}
