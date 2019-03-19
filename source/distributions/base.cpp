#include "distributions/base.h"

Korali::BaseDistribution::BaseDistribution(size_t seed) : _seed(seed)
{
  _range = gsl_rng_alloc (gsl_rng_default);
  gsl_rng_set(_range, _seed);
}
