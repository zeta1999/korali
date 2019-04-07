#ifndef _KORALI_EXPONENTIAL_H_
#define _KORALI_EXPONENTIAL_H_

#include "parameters/base/base.h"

namespace Korali::Parameter
{

class Exponential : public Korali::Parameter::Base
{
 private:
  double _mean;

 public:
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();

  // Serialization Methods
  nlohmann::json getConfiguration();
  void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_EXPONENTIAL_H_
