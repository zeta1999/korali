#ifndef _KORALI_BASECONDUIT_H_
#define _KORALI_BASECONDUIT_H_

#include <stdlib.h>

namespace Korali::Conduit
{

class Base {
 public:

 Base();

 virtual void initialize() = 0;
 virtual void evaluateSample(size_t sampleId) = 0;
 virtual void checkProgress() = 0;

 virtual double* getSampleArrayPointer() = 0;
 virtual double* getFitnessArrayPointer() = 0;
};

class Conduit;

} // namespace Korali

#endif // _KORALI_BASECONDUIT_H_
