#include "korali.h"

void Korali::Parameter::Base::initialize(int seed)
{
 _range = gsl_rng_alloc (gsl_rng_default);
 gsl_rng_set(_range, seed);
}
