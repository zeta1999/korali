#ifndef _KORALI_BASEDISTRIBUTION_H_
#define _KORALI_BASEDISTRIBUTION_H_

#include <vector>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

namespace Korali
{

class BaseDistribution
{
 protected:

 gsl_rng* _range;
 size_t _seed;

 public:
 BaseDistribution(size_t seed);
 virtual double getDensity(double x) = 0;
 virtual double getDensityLog(double x) = 0;
 virtual double getRandomNumber() = 0;
};

} // namespace Korali

#endif // _KORALI_BASEDISTRIBUTION_H_
