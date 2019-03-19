#ifndef _KORALI_PARAMETER_H_
#define _KORALI_PARAMETER_H_

#include <vector>
#include <limits>
#include <string>
#include "distributions/base.h"

namespace Korali
{

class Parameter
{
 public:

 std::string _name;
 std::string _type;
 double _a; // First parameter of distribution. Could be mean (gaussian) or min (uniform)
 double _b; // First parameter of distribution. Could be sigma (gaussian) or max (uniform)
 double _lowerBound;
 double _upperBound;
 double _initialX;
 double _initialStdDev;
 double _minStdDevChange;
 size_t _seed;

 Parameter();
 Parameter(std::string name);

 double getRandomNumber();
 double getDensityLog(double x);
 double getDensity(double x);

 void setName(std::string name);
 void setPriorDistribution(std::string type, double a, double b = 0.0);
 void setBounds(double lowerBound, double upperBound);
 void setInitialX(double initialX);
 void setInitialStdDev(double initialStdDev);
 void setMinStdDevChange(double minStdDevChange);
 void initialize(int seed);
 void checkDistribution();
 void checkBounds();

 private:
 BaseDistribution* _prior;

};


} // Namespace Korali

#endif // _KORALI_PARAMETER_H_
