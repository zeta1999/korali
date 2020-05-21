#ifndef _MODEL_CPP_
#define _MODEL_CPP_

#include "korali.hpp"

#include "load_data.hpp"
#include "model.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <typeinfo>
#include <random>

/*
Model 4:
 (see python tutorial, in basic/...)
*/




ConditionalDistribution4::ConditionalDistribution4(){

        _p = simplePopulationData();

};

//     See also the python tutorial, in basic/... . Here we only know the part
//         p(data | latent), which is a product of Gaussians:
//
//          Model 3:
//            draw psi_i ~ N(theta, omega**2)
//            draw x_i ~ N(psi_i, sigma**2)
//
void ConditionalDistribution4::conditional_p(korali::Sample& s)
{
      std::vector<double> latentVariables = s["Latent Variables"];

      double mean = latentVariables[0];
      std::vector<double> mean_vec({mean});
      double sigma = _p.sigma;

      // log(p(data | mean, sigma ))
      double logp = 0;
      for (size_t i=0; i < _p.nIndividuals; i++){
        double pt = _p.data[i][0];
        std::vector<double> pt_vec({pt});


        double p = univariate_gaussian_probability(mean_vec, sigma, pt_vec);

        logp += log(p);
      }

      s['Conditional LogLikelihood'] = logp;
};



#endif
