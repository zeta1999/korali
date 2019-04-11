#ifndef _KORALI_GAMMA_H_
#define _KORALI_GAMMA_H_

#include "parameters/base/base.h"

namespace Korali::Parameter
{

class Gamma : public Korali::Parameter::Base
{
 private:
  double _shape;
  double _rate;

 public:
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();

  // Constructor / Destructor
  Gamma(nlohmann::json& js, int seed);
  ~Gamma();

  // Serialization Methods
  nlohmann::json getConfiguration();
  void setConfiguration(nlohmann::json& js);
};

} // namespace Korali

#endif // _KORALI_GAMMA_H_
