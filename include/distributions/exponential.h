#ifndef _KORALI_EXPONENTIAL_H_
#define _KORALI_EXPONENTIAL_H_

#include "distributions/base.h"

namespace Korali
{

class Exponential : public BaseDistribution
{
 private:
  double _mean;

 public:
  Exponential(double mean);
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
};

} // namespace Korali

#endif // _KORALI_EXPONENTIAL_H_
