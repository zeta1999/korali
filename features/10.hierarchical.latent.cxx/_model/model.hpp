#ifndef _MODEL_HPP_
#define _MODEL_HPP_

#include "korali.hpp"

#include "load_data.hpp"
#include "utils.hpp"

#include <cmath>
#include <random>
#include <vector>

class HierarchicalDistribution4
{
  public:
  HierarchicalDistribution4();
  void conditional_p(korali::Sample &k);
  pointsInfoStruct _p;
};

class HierarchicalDistribution5
{
  public:
  HierarchicalDistribution5();
  void conditional_p(korali::Sample &k);
  pointsInfoStructAdvanced _p;
};

#endif
