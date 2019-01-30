#ifndef _PRIOR_H_
#define _PRIOR_H_

#include <vector>
#include "distribution.h"

namespace Korali
{

class Prior
{
 private:

 std::vector<Distribution*> _dimArray;
 double* _randomNumber;

 public:
 void addDimension(Distribution* d);
 int getDimCount();
 void initialize(int dimCount);

 double getDensity(double* x);
 double getDensityLog(double* x);
 double* getRandomNumber();
};


} // Namespace Korali

#endif // _PRIOR_H_
