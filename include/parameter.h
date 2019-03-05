#ifndef _PARAMETER_H_
#define _PARAMETER_H_

#include <vector>
#include <limits>
#include <string>
#include "distribution.h"

namespace Korali
{

class Parameter
{
 public:

 bool _priorSet;
 bool _boundsSet;

 std::string _name;
 double _lowerBound;
 double _upperBound;
 double _initialX;
 double _initialStdDev;
 double _minStdDevChange;
 int _seed;

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
 void setSeed(int seed);

 private:
 Distribution* _prior;

};


} // Namespace Korali

#endif // _PARAMETER_H_
