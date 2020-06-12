#ifndef _LOADDATA_H_
#define _LOADDATA_H_
#include "korali.hpp"

#include <stdlib.h>
#include <vector>

typedef struct pointsInfoStruct
{
  std::vector<std::vector<double>> data;
  size_t nIndividuals;
  size_t nSamplesEach;
  double sigma;
  double omega;
} pointsInfo;

pointsInfoStruct &simplePopulationData();

void reset_points(pointsInfoStruct &__p, std::vector<std::vector<double>> new_data, int new_nSamplesEach);

#endif // _HEAT2D_H_
