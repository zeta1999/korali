#ifndef _KORALI_EXPONENTIAL_H_
#define _KORALI_EXPONENTIAL_H_

#include "parameters/base/base.h"

namespace Korali::Parameter
{

class Exponential : public Korali::Parameter::Base
{
 private:
  double _loc;
  double _mean;

 public:

  Exponential();
  Exponential(double mean);
  Exponential(double loc, double mean);
  Exponential(double mean, size_t seed);
  Exponential(double loc, double mean, size_t seed);

  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();

  void printDetails() override;

  // Serialization Methods
  nlohmann::json getConfiguration();
  void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_EXPONENTIAL_H_
