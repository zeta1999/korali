#ifndef _KORALI_H_
#define _KORALI_H_

#include "problems/direct.h"
#include "problems/likelihood.h"
#include "problems/posterior.h"

#include "parameters/exponential.h"
#include "parameters/gamma.h"
#include "parameters/gaussian.h"
#include "parameters/uniform.h"

#include "solvers/cmaes.h"
#include "solvers/tmcmc.h"

#include "conduits/single.h"
#include "conduits/upcxx.h"

namespace Korali
{

class Korali {
public:



};

extern Korali k;

} // namespace Korali

#endif // _KORALI_H_
