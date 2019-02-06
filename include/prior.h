#ifndef _PRIOR_H_
#define _PRIOR_H_

#include <vector>
#include <limits>
#include <string>
#include "distribution.h"

namespace Korali
{

class Prior
{
 private:
 public:

 Distribution* _prior;
 double _lowerBound;
 double _upperBound;
 double _initialX;
 double _initialStdDev;
 double _minStdDevChange;
 int _seed;

 Prior();
 void setPriorDistribution(std::string type, double a, double b = 0.0);
 Distribution* getPriorDistribution();
 void setBounds(double lowerBound, double upperBound);
 void setInitialX(double initialX);
 void setInitialStdDev(double initialStdDev);
 void setMinStdDevChange(double minStdDevChange);
 void setSeed(int seed);

};


} // Namespace Korali

#endif // _PRIOR_H_
