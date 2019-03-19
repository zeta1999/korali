#ifndef _KORALI_GAMMA_H_
#define _KORALI_GAMMA_H_

#include "distributions/base.h"

namespace Korali
{

class Gamma : public BaseDistribution
{
 private:
  double _shape;
  double _rate;

 public:
  Gamma(double shape, double rate);
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
};

} // namespace Korali

#endif // _KORALI_GAMMA_H_
