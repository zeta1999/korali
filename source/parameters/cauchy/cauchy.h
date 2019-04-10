#ifndef _KORALI_CAUCHY_H_
#define _KORALI_CAUCHY_H_

#include "parameters/base/base.h"

namespace Korali::Parameter
{

class Cauchy : public Korali::Parameter::Base
{
 private:
  double _aux;
  double _loc;
  double _scale;


 public:

  Cauchy();
  Cauchy(double scale);
  Cauchy(double loc, double scale);
  Cauchy(double scale, size_t seed);
  Cauchy(double loc, double scale, size_t seed);

  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();

  void printDetails() override;

  // Serialization Methods
  nlohmann::json getConfiguration();
  void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_CAUCHY_H_
