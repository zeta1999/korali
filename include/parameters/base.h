#ifndef _KORALI_BASEPARAMETER_H_
#define _KORALI_BASEPARAMETER_H_

#include <string>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "json.hpp"

namespace Korali::Parameter
{

class Base
{
 public:
 Base();
 gsl_rng* _range;

 std::string _name;
 double _lowerBound;
 double _upperBound;

 // These are CMA-ES Specific, but could be used for other methods in the future
 double _initialValue;
 double _initialStdDev;
 double _minStdDevChange;

 void initialize(int seed);

 virtual double getDensity(double x) = 0;
 virtual double getDensityLog(double x) = 0;
 virtual double getRandomNumber() = 0;

 // Serialization Methods
 virtual nlohmann::json getConfiguration();
 virtual void setConfiguration(nlohmann::json js);
};

} // namespace Korali

#endif // _KORALI_BASEPARAMETER_H_
