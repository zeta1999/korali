
#include "load_data.hpp"
#include "string.h"
#include <limits>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

pointsInfo __p;

pointsInfoStruct &simplePopulationData()
{
  FILE *problemFile;
  FILE *problemFile2;
  size_t nIndividuals = 0;
  size_t nSamplesEach = 0;
  double sigma = 0;
  double omega = 0;

  printf("Loading data from data_925.in ... \n");
  problemFile = fopen("model/data_925.in", "r");
  if (!problemFile)
  {
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    std::cout << "Current workdir: " << cwd << std::endl;
    fprintf(stderr, "[Error] File not found: model/data_925.in. Make sure you run this script from the 'tutorials/advanced/10.hierarchical.latent.cxx' directory. ");
    exit(-1);
  }
  problemFile2 = fopen("data_925.in", "r");
  fscanf(problemFile, "%lu", &nIndividuals);
  fscanf(problemFile, "%lu", &nSamplesEach);
  fscanf(problemFile, "%le", &sigma);
  fscanf(problemFile, "%le", &omega);

  __p.data.resize(nIndividuals);

  for (size_t i = 0; i < nIndividuals; i++)
  {
    __p.data[i].resize(nSamplesEach);
    double val;
    for (size_t j = 0; j < nSamplesEach; j++)
    {
      fscanf(problemFile, "%le ", &val);
      __p.data[i][j] = val;
    }
  }
  fclose(problemFile);
  __p.nIndividuals = nIndividuals;
  __p.nSamplesEach = nSamplesEach;
  __p.sigma = sigma;
  __p.omega = omega;

  return __p;
};

void reset_points(pointsInfoStruct &__p, std::vector<std::vector<double>> new_data, int new_nSamplesEach, double new_sigma, double new_omega)
{
  __p.nIndividuals = new_data.size();
  __p.nSamplesEach = new_nSamplesEach;
  __p.sigma = new_sigma;
  __p.omega = new_omega;
  __p.data = new_data;
  for (std::vector<double> pt : new_data)
    assert(pt.size() == new_nSamplesEach && "... I think the inner dimension was over the samples per each individual. This is a primitive example with dimension == 1.");
  return;
}
