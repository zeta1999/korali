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
 private:
 public:

 bool _priorSet;
 bool _boundsSet;

 Distribution* _prior;
 std::string _name;
 double _lowerBound;
 double _upperBound;
 double _initialX;
 double _initialStdDev;
 double _minStdDevChange;
 int _seed;

 Parameter();
 Parameter(std::string name);
 void setName(std::string name);
 void setPriorDistribution(std::string type, double a, double b = 0.0);
 Distribution* getPriorDistribution();
 void setBounds(double lowerBound, double upperBound);
 void setInitialX(double initialX);
 void setInitialStdDev(double initialStdDev);
 void setMinStdDevChange(double minStdDevChange);
 void setSeed(int seed);

};


} // Namespace Korali

#endif // _PARAMETER_H_
