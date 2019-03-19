#include "parameters/base.h"

Korali::Parameter::Base::Base()
{
 _name = "NoName";
 _initialX = 0.0;
 _initialStdDev = 0.1;
 _minStdDevChange = 0.0;
 _lowerBound = 0.0;
 _upperBound = -1.0;
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
}
