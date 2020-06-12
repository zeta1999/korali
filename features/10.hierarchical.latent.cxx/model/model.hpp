#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include "korali.hpp"
#include "load_data.hpp"
#include "utils.hpp"

#include <cmath>
#include <random>
#include <vector>

class ConditionalDistribution4
{
  public:
  ConditionalDistribution4();
  void conditional_p(korali::Sample &k);
  pointsInfoStruct _p;
};

#endif
