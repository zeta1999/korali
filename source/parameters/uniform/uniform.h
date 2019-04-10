#ifndef _KORALI_UNIFORM_H_
#define _KORALI_UNIFORM_H_

#include "parameters/base/base.h"

namespace Korali::Parameter
{

class Uniform : public Korali::Parameter::Base
{
 private:
   double _aux;
   
 public:

  Uniform();
  Uniform(double _lowerBound, double _upperBound);
  Uniform(double _lowerBound, double _upperBound, size_t seed);

  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();

  void printDetails() override;

  // Serialization Methods
  nlohmann::json getConfiguration();
  void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_DISTRIBUTION_H_
