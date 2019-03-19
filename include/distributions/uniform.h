#ifndef _KORALI_UNIFORM_H_
#define _KORALI_UNIFORM_H_

#include "distributions/base.h"

namespace Korali
{

class Uniform : public BaseDistribution
{
 private:
  double _min;
  double _max;

 public:
  Uniform(double min, double max);
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
};

} // namespace Korali

#endif // _KORALI_DISTRIBUTION_H_
