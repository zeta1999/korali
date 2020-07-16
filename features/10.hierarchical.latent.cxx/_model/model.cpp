#ifndef _MODEL_CPP_
#define _MODEL_CPP_

#include "korali.hpp"

#include "load_data.hpp"
#include "model.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <typeinfo>
#include <vector>

/*
Model 4:
 (see python tutorial, in basic/...)
*/

HierarchicalDistribution4::HierarchicalDistribution4()
{
  _p = simplePopulationData();
};

//     See also the python tutorial, in basic/... . Here we only know the part
//         p(data | latent), which is a product of Gaussians:
//
//          Model 3:
//            draw psi_i ~ N(theta, omega**2)
//            draw x_i ~ N(psi_i, sigma**2)
//
void HierarchicalDistribution4::conditional_p(korali::Sample &s)
{
  std::vector<double> latentVariables = s["Latent Variables"];
  std::vector<double> dataPoint = s["Data Point"];
  assert(latentVariables.size() == 1); // nr latent space dimensions = 1

  double sigma = _p.sigma;

  // log(p(data | mean, sigma ))
  double logp = 0;
  // for (size_t i=0; i < _p.nIndividuals; i++){
  //double pt = _p.data[i][0];
  double pt = dataPoint[0]; // in this example there is only one data dimension, too
  //assert (pt == _p.data[i][0]); // did we set the data correctly?
  std::vector<double> pt_vec({pt});
  //        double mean = latentVariables[0];
  //        std::vector<double> mean_vec({mean});
  assert(latentVariables.size() == 1); // In this example the dimension is 1
  double p = univariate_gaussian_probability(latentVariables, sigma, pt_vec);

  logp += log(p);
  // }

  s["Conditional LogLikelihood"] = logp;
};

/*
Model 5:
 - multiple dimensions
 - multiple distribution types
 - latent variable coordinates are correlated
 - p(datapoint | latent) is still a normal distribution N(latent, sigma**2)
*/
HierarchicalDistribution5::HierarchicalDistribution5()
{
  _p = populationData();
};

void HierarchicalDistribution5::conditional_p(korali::Sample &s)
{
  std::vector<double> latentVariables = s["Latent Variables"];
  std::vector<double> dataPoint = s["Data Point"];
  assert(latentVariables.size() == _p.nDimensions);
  assert(dataPoint.size() == _p.nDimensions);

  double sigma = _p.sigma;

  // log(p(data | mean=latent variable, sigma ))
  double logp = 0;
  for (size_t i = 0; i < _p.nDimensions; i++)
  {
    double pt = dataPoint[i];
    double mean = latentVariables[i];
    std::vector<double> pt_vec({pt});
    std::vector<double> mean_vec({mean});
    double p = univariate_gaussian_probability(mean_vec, sigma, pt_vec);
    logp += log(p);
  }

  s["Conditional LogLikelihood"] = logp;
};

#endif
