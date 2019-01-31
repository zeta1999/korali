#ifndef _DIMENSION_H_
#define _DIMENSION_H_

#include <vector>
#include <limits>
#include <string>
#include "distribution.h"

namespace Korali
{

class Dimension
{
 private:

 Distribution* _prior;
 double _lowerBound;
 double _upperBound;
 double _initialX;
 double _typicalX;
 double _initialStdDev;
 int _seed;

 public:

 Dimension();
 void setPriorDistribution(std::string type, double a, double b = 0.0);
 Distribution* getPriorDistribution();
 void setBounds(double lowerBound, double upperBound);
 void setInitialX(double initialX);
 void setTypicalX(double typicalX);
 void setInitialStdDev(double initialStdDev);
 void setSeed(int seed);

};


} // Namespace Korali

#endif // _DIMENSION_H_
