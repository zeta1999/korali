#ifndef _KORALI_GAUSSIAN_H_
#define _KORALI_GAUSSIAN_H_

#include "parameters/base/base.h"

namespace Korali::Parameter
{

class Gaussian : public Korali::Parameter::Base
{
 private:
  double _mean;
  double _sigma;

 public:
  double getDensity(double x);
  double getDensityLog(double x);
  double getRandomNumber();
  static double logLikelihood(double sigma, int nData, double* x, double* u);
  void setDistribution(double mean, double sigma) { _mean = mean; _sigma = sigma; };

  // Constructor / Destructor
  Gaussian(nlohmann::json& js, int seed);
  ~Gaussian();

  // Serialization Methods
  nlohmann::json getConfiguration();
  void setConfiguration(nlohmann::json& js);
};

} // namespace Korali

#endif // _KORALI_GAUSSIAN_H_
